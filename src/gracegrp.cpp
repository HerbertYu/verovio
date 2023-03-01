/////////////////////////////////////////////////////////////////////////////
// Name:        gracegrp.cpp
// Author:      Klaus Rettinghaus
// Created:     2018
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "gracegrp.h"

//----------------------------------------------------------------------------

#include <cassert>

//----------------------------------------------------------------------------

#include "beam.h"
#include "chord.h"
#include "editorial.h"
#include "functorparams.h"
#include "note.h"
#include "rest.h"
#include "space.h"
#include "vrv.h"

//----------------------------------------------------------------------------

#include "MidiFile.h"
#include "midiext.h"

namespace vrv {

//----------------------------------------------------------------------------
// GraceGrp
//----------------------------------------------------------------------------

static const ClassRegistrar<GraceGrp> s_factory("graceGrp", GRACEGRP);

GraceGrp::GraceGrp() : LayerElement(GRACEGRP, "gracegrp-"), AttColor(), AttGraced(), AttGraceGrpLog()
{
    this->RegisterAttClass(ATT_COLOR);
    this->RegisterAttClass(ATT_GRACED);
    this->RegisterAttClass(ATT_GRACEGRPLOG);

    this->Reset();
}

GraceGrp::~GraceGrp() {}

void GraceGrp::Reset()
{
    LayerElement::Reset();
    this->ResetColor();
    this->ResetGraced();
    this->ResetGraceGrpLog();
}

bool GraceGrp::IsSupportedChild(Object *child)
{
    if (child->Is(BEAM)) {
        assert(dynamic_cast<Beam *>(child));
    }
    else if (child->Is(CHORD)) {
        assert(dynamic_cast<Chord *>(child));
    }
    else if (child->Is(NOTE)) {
        assert(dynamic_cast<Note *>(child));
    }
    else if (child->Is(REST)) {
        assert(dynamic_cast<Rest *>(child));
    }
    else if (child->Is(SPACE)) {
        assert(dynamic_cast<Space *>(child));
    }
    else if (child->IsEditorialElement()) {
        assert(dynamic_cast<EditorialElement *>(child));
    }
    else {
        return false;
    }
    return true;
}

int GraceGrp::GenerateMIDIEnd(FunctorParams *functorParams)
{
    GenerateMIDIParams *params = vrv_params_cast<GenerateMIDIParams *>(functorParams);
    assert(params);

    // Handling of Nachschlag
    if (!params->m_graceNotes.empty() && (this->GetAttach() == graceGrpLog_ATTACH_pre) && !params->m_accentedGraceNote
        && params->m_lastNote) {
        double startTime = params->m_totalTime + params->m_lastNote->GetScoreTimeOffset();
        const double graceNoteDur = UNACC_GRACENOTE_DUR * params->m_currentTempo / 60000.0;
        const double totalDur = graceNoteDur * params->m_graceNotes.size();
        startTime -= totalDur;
        startTime = std::max(startTime, 0.0);

        const int channel = params->m_midiChannel;
        int velocity = MIDI_VELOCITY;
        if (params->m_lastNote->HasVel()) velocity = params->m_lastNote->GetVel();
        const int tpq = params->m_midiFile->getTPQ();

        int index = 0;
        for (auto iter = params->m_graceNotes.begin(); iter != params->m_graceNotes.end(); iter++, index++) {
            if (params->m_midiExt) {
                auto object = params->m_graceRefs[index];
                if (object->Is(CHORD)) {
                    auto chord = dynamic_cast<Chord*>(object);
                    const ListOfObjects &notes = chord->GetList(chord);
                    for (Object *obj : notes) {
                        Note *note = vrv_cast<Note *>(obj);
                        assert(note);
                        params->m_midiExt->AddNote(startTime * tpq, note);
                    }
                } else if (object->Is(NOTE)) {
                    Note *note = vrv_cast<Note *>(object);
                    assert(note);
                    params->m_midiExt->AddNote(startTime * tpq, note);
                }
            }
            const MIDIChord &chord = *iter;
            const double stopTime = startTime + graceNoteDur;
            for (int pitch : chord.pitches) {
                params->m_midiFile->addNoteOn(params->m_midiTrack, startTime * tpq, channel, pitch, velocity);
                params->m_midiFile->addNoteOff(params->m_midiTrack, stopTime * tpq, channel, pitch);
            }
            startTime = stopTime;
        }

        params->m_graceNotes.clear();
        params->m_graceRefs.clear();
    }
    return FUNCTOR_CONTINUE;
}

} // namespace vrv

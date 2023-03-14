/////////////////////////////////////////////////////////////////////////////
// Name:        MidiExt.cpp
// Author:      Herbert
// Created:     23/04/2022
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "midiext.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "functorparams.h"
#include "measure.h"
#include "note.h"
#include "rest.h"
#include "staff.h"
#include "system.h"
#include "page.h"
#include "elementpart.h"
#include "artic.h"
#include "vrv.h"
#include "stem.h"
#include "tuplet.h"
#include "layer.h"
#include "midi/MidiFile.h"

namespace vrv {

//----------------------------------------------------------------------------
// MidiExt
//----------------------------------------------------------------------------

    MidiExt::MidiExt() {
        this->Reset();
    }

    MidiExt::~MidiExt() {}

    void MidiExt::Reset() {
        m_entries.clear();
        m_measureTicks.clear();
    }

    void MidiExt::AddNote(int tick, Note *object) {
        if (m_entries.count(tick) == 0) {
            MidiExtEntry emptyEntry;
            m_entries[tick] = emptyEntry;
        }
        MidiExtEntry *entry = &m_entries[tick];

        DurationInterface *interface = object->GetDurationInterface();
        assert(interface);

        auto staff = dynamic_cast<Staff *>(object->GetFirstAncestor(STAFF));
        auto layer = dynamic_cast<Layer *>(object->GetFirstAncestor(LAYER));
        int staffNo = staff ? staff->GetN() : 0;
        auto beam = dynamic_cast<Beam *>(object->GetFirstAncestor(vrv::BEAM));
        auto tuplet = dynamic_cast<Tuplet *>(object->GetFirstAncestor(vrv::TUPLET));
        auto accid = dynamic_cast<Accid *>(object->FindDescendantByType(vrv::ACCID));
        auto atric = dynamic_cast<Artic *>(object->FindDescendantByType(vrv::ARTIC));
        auto dots = dynamic_cast<Dots *>(object->FindDescendantByType(vrv::DOTS));
        auto stem = dynamic_cast<Stem *>(object->FindDescendantByType(vrv::STEM));
        auto flag = stem ? dynamic_cast<Flag *>(stem->FindDescendantByType(vrv::FLAG)) : nullptr;
        auto chord = dynamic_cast<Chord *>(object->GetFirstAncestor(CHORD));
        auto chordStem = chord ? dynamic_cast<Stem *>(chord->FindDescendantByType(vrv::STEM)) : nullptr;
        auto chordFlag = chord ? dynamic_cast<Flag *>(chord->FindDescendantByType(vrv::FLAG)) : nullptr;
        auto chordDots = chord ? dynamic_cast<Dots *>(chord->FindDescendantByType(vrv::DOTS)) : nullptr;

        // Store the element ID in list to turn on at given time - note or rest
        Note *note = dynamic_cast<Note *>(object);
        auto pitch = note->GetMIDIPitch();
        auto uuid = object->GetID();
        std::vector<std::string> elements;
        elements.emplace_back(uuid);
        if (beam) {
            elements.emplace_back(beam->GetID());
        }
        if (tuplet) {
            elements.emplace_back(tuplet->GetID());
        }
        if (accid) {
            elements.emplace_back(accid->GetID());
        }
        if (atric) {
            elements.emplace_back(atric->GetID());
        }
        if (dots) {
            elements.emplace_back(dots->GetID());
        }
        if (stem) {
            elements.emplace_back(stem->GetID());
        }
        if (flag) {
            elements.emplace_back(flag->GetID());
        }
        if (chordFlag) {
            elements.emplace_back(chordFlag->GetID());
        }
        if (chordStem) {
            elements.emplace_back(chordStem->GetID());
        }
        if (chordDots) {
            elements.emplace_back(chordDots->GetID());
        }
        auto duration = (int)(object->GetDurationInterface()->GetScoreTimeDuration() * 120);
        Measure *measure = dynamic_cast<Measure *>(object->GetFirstAncestor(MEASURE));
        if (measure) {
            auto measureNo = -1;
            try {
                measureNo = std::stoi(measure->GetN()) - 1;
            } catch (std::exception& e) {
                ;
            }

            entry->measureNo = measureNo;
            
            entry->pitches.emplace(pitch, MidiExtPitch{duration, staffNo, elements});
        }

        Page *page = dynamic_cast<Page *>(object->GetFirstAncestor(PAGE));
        if (page) {
            entry->pageNo = page->GetPageIdx();
        }
    }

    MidiExtEntry *MidiExt::GetTimeEntry(int tick) {
        auto iter = m_entries.find(tick);
        return iter != m_entries.end() ? &iter->second : nullptr;
    }

    void MidiExt::AddMeasure(int tick, int duration, Measure *measure) {
        auto system = dynamic_cast<System *>(measure->GetFirstAncestor(SYSTEM));
        if (system) {
            auto uuid = system->GetID();
            if (m_systemUuid.count(uuid) == 0) {
                m_systemUuid[uuid] = m_systemUuid.size();
            }
            auto measureNo = -1;
            try {
                measureNo = std::stoi(measure->GetN()) - 1;
                m_measureTicks[tick] = MidiExtMeasure{
                        measureNo,
                        duration,
                        m_systemUuid[uuid]
                };
            } catch (std::exception& e) {
                fprintf(stdout, "[MidiExt]measure no error:%s\n", measure->GetN().c_str());
            }
        }
    }

    void MidiExt::CopyMeasures(int fromTick, int endTick, int addTick) {
        auto iter = m_measureTicks.find(fromTick);
        while (iter != m_measureTicks.end() && iter->first < endTick) {
            m_measureTicks[iter->first + addTick] = iter->second;
            iter++;
        }
    }

    void MidiExt::CopyTimeEntry(int fromTick, int endTick, int addTick) {
        auto iter = std::find_if(m_entries.begin(), m_entries.end(),
                                 [fromTick](const std::pair<int, MidiExtEntry> &entry) {
                                     return entry.first >= fromTick;
                                 });
        while (iter != m_entries.end() && iter->first < endTick) {
            m_entries[iter->first + addTick] = iter->second;
            iter++;
        }
    }

    const std::map<int, MidiExtMeasure> &MidiExt::GetMeasures() const {
        return m_measureTicks;
    }

    const std::map<int, MidiExtEntry> &MidiExt::GetEntries() const {
        return m_entries;
    }

    const std::map<std::string, int> &MidiExt::GetSystems() const {
        return m_systemUuid;
    }

} // namespace vrv

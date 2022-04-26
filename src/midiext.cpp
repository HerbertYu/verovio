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
        m_map.clear();
        m_measureTicks.clear();
    }

    void MidiExt::AddNote(int tick, Note *object) {
        if (m_map.count(tick) == 0) {
            MidiExtEntry emptyEntry;
            m_map[tick] = emptyEntry;
        }
        MidiExtEntry *entry = &m_map[tick];

        DurationInterface *interface = object->GetDurationInterface();
        assert(interface);

        auto staff = dynamic_cast<Staff *>(object->GetFirstAncestor(STAFF));
        int staffNo = staff ? staff->GetN() : 0;
        auto beam = dynamic_cast<Beam *>(object->GetFirstAncestor(vrv::BEAM));
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
        auto uuid = object->GetUuid();
        std::vector<std::string> elements;
        elements.emplace_back(uuid);
        if (beam) {
            elements.emplace_back(beam->GetUuid());
        }
        if (accid) {
            elements.emplace_back(accid->GetUuid());
        }
        if (atric) {
            elements.emplace_back(atric->GetUuid());
        }
        if (dots) {
            elements.emplace_back(dots->GetUuid());
        }
        if (stem) {
            elements.emplace_back(stem->GetUuid());
        }
        if (flag) {
            elements.emplace_back(flag->GetUuid());
        }
        if (chordFlag) {
            elements.emplace_back(chordFlag->GetUuid());
        }
        if (chordStem) {
            elements.emplace_back(chordStem->GetUuid());
        }
        if (chordDots) {
            elements.emplace_back(chordDots->GetUuid());
        }
        entry->notesOn.emplace(pitch, std::make_pair(staffNo, elements));
        Measure *measure = dynamic_cast<Measure *>(object->GetFirstAncestor(MEASURE));
        if (measure) {
            entry->measureNo = std::stoi(measure->GetN()) - 1;
        }

        Page *page = dynamic_cast<Page *>(object->GetFirstAncestor(PAGE));
        if (page) {
            entry->pageNo = page->GetPageIdx();
        }
        
        fprintf(stdout, "[MidiExt]addNote track:%d,tick:%d,note:%d\n", staffNo, tick, pitch);
    }

    MidiExtEntry *MidiExt::GetTimeEntry(int tick) {
        auto iter = m_map.find(tick);
        return iter != m_map.end() ? &iter->second : nullptr;
    }

    void MidiExt::AddMeasure(int tick, int measure) {
        m_measureTicks[tick] = measure;
    }

    void MidiExt::CopyMeasures(int fromTick, int endTick, int addTick) {
        auto iter = m_measureTicks.find(fromTick);
        while (iter != m_measureTicks.end() && iter->first < endTick) {
            m_measureTicks[iter->first + addTick] = iter->second;
            iter++;
        }
    }

    void MidiExt::CopyTimeEntry(int fromTick, int endTick, int addTick) {
        auto iter = std::find_if(m_map.begin(), m_map.end(), [fromTick](const std::pair<int, MidiExtEntry> &entry) {
            return entry.first >= fromTick;
        });
        while (iter != m_map.end() && iter->first < endTick) {
            m_map[iter->first + addTick] = iter->second;
            iter++;
        }
    }

    const std::map<int, int> &MidiExt::GetMeasures() const {
        return m_measureTicks;
    }

    const std::map<int, MidiExtEntry> &MidiExt::GetEntries() const {
        return m_map;
    }
} // namespace vrv

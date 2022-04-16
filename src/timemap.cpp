/////////////////////////////////////////////////////////////////////////////
// Name:        timemap.cpp
// Author:      Laurent Pugin
// Created:     29/01/2022
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "timemap.h"

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

namespace vrv {

//----------------------------------------------------------------------------
// Timemap
//----------------------------------------------------------------------------

Timemap::Timemap()
{
    this->Reset();
}

Timemap::~Timemap() {}

void Timemap::Reset()
{
    m_map.clear();
}

void Timemap::AddEntry(Object *object, GenerateTimemapParams *params)
{
    assert(object);
    assert(params);

    // It is a bit weird to have a timemap parameter, but we need it to call this method from the functor.
    // Just make sure they are the same because below we use m_map (and not params->m_timemap)
    assert(params->m_timemap == this);

    TimemapEntry emptyEntry;

    if (object->Is({ NOTE, REST })) {
        DurationInterface *interface = object->GetDurationInterface();
        assert(interface);

        double realTimeStart = params->m_realTimeOffsetMilliseconds + interface->GetRealTimeOnsetMilliseconds();
        double scoreTimeStart = params->m_scoreTimeOffset + interface->GetScoreTimeOnset();

        double realTimeEnd = params->m_realTimeOffsetMilliseconds + interface->GetRealTimeOffsetMilliseconds();
        double scoreTimeEnd = params->m_scoreTimeOffset + interface->GetScoreTimeOffset();

        bool isRest = (object->Is(REST));

        /*********** start values ***********/

        if (m_map.count(realTimeStart) == 0) {
            m_map[realTimeStart] = emptyEntry;
        }

        auto staff = dynamic_cast<Staff*>(object->GetFirstAncestor(STAFF));
        int staffNo = staff ? staff->GetN() : 0;
        auto beam = dynamic_cast<Beam *>(object->GetFirstAncestor(vrv::BEAM));
        auto accid = dynamic_cast<Accid *>(object->FindDescendantByType(vrv::ACCID));
        auto atric = dynamic_cast<Artic *>(object->FindDescendantByType(vrv::ARTIC));
        auto dots = dynamic_cast<Dots *>(object->FindDescendantByType(vrv::DOTS));
        auto stem = dynamic_cast<Stem *>(object->FindDescendantByType(vrv::STEM));
        auto flag = stem ? dynamic_cast<Flag *>(stem->FindDescendantByType(vrv::FLAG)) : nullptr;
        auto chord = dynamic_cast<Chord*>(object->GetFirstAncestor(CHORD));
        auto chordStem = chord ? dynamic_cast<Stem *>(chord->FindDescendantByType(vrv::STEM)) : nullptr;
        auto chordFlag = chord ? dynamic_cast<Flag *>(chord->FindDescendantByType(vrv::FLAG)) : nullptr;
        auto chordDots = chord ? dynamic_cast<Dots *>(chord->FindDescendantByType(vrv::DOTS)) : nullptr;

        TimemapEntry *startEntry = &m_map.at(realTimeStart);

        // Should check if value for realTimeStart already exists and if so, then
        // ensure that it is equal to scoreTimeStart:
        startEntry->qstamp = scoreTimeStart;

        // Store the element ID in list to turn on at given time - note or rest
        if (!isRest) {
            Note* note = dynamic_cast<Note*>(object);
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
            if (dots){
                elements.emplace_back(dots->GetUuid());
            }
            if (stem) {
                elements.emplace_back(stem->GetUuid());
            }
            if (flag) {
                elements.emplace_back(flag->GetUuid());
            }
            if(chordFlag){
                elements.emplace_back(chordFlag->GetUuid());
            }
            if(chordStem){
                elements.emplace_back(chordStem->GetUuid());
            }
            if(chordDots){
                elements.emplace_back(chordDots->GetUuid());
            }
            startEntry->notesOn.emplace(pitch, std::make_pair(staffNo, elements));
        }
        if (isRest) {
            startEntry->restsOn.push_back(object->GetUuid());
        }

        Measure* measure = dynamic_cast<Measure*>(object->GetFirstAncestor(MEASURE));
        if (measure) {
            startEntry->measureNo = std::stoi(measure->GetN());
        }

        System* system = dynamic_cast<System*>(object->GetFirstAncestor(SYSTEM));
        if (system) {
            startEntry->systemNo = system->GetSystemIdx();
        }
        Page* page = dynamic_cast<Page*>(object->GetFirstAncestor(PAGE));
        if (page) {
            startEntry->pageNo = page->GetPageIdx();
        }

        // Also add the tempo the
        startEntry->tempo = params->m_currentTempo;

        /*********** end values ***********/

        if (m_map.count(realTimeEnd) == 0) {
            m_map[realTimeEnd] = emptyEntry;
        }
        TimemapEntry *endEntry = &m_map.at(realTimeEnd);

        // Should check if value for realTimeEnd already exists and if so, then
        // ensure that it is equal to scoreTimeEnd:
        endEntry->qstamp = scoreTimeEnd;

        // Store the element ID in list to turn off at given time - notes or rest
        if (!isRest) {
            Note* note = dynamic_cast<Note*>(object);
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
            if (dots){
                elements.emplace_back(dots->GetUuid());
            }
            if (stem) {
                elements.emplace_back(stem->GetUuid());
            }
            if (flag) {
                elements.emplace_back(flag->GetUuid());
            }
            if(chordFlag){
                elements.emplace_back(chordFlag->GetUuid());
            }
            if(chordStem){
                elements.emplace_back(chordStem->GetUuid());
            }
            if(chordDots){
                elements.emplace_back(chordDots->GetUuid());
            }
            endEntry->notesOff.emplace(pitch, std::make_pair(staffNo, elements));
        }
        if (isRest) {
            endEntry->restsOff.push_back(object->GetUuid());
        }

        if (measure) {
            endEntry->measureNo = std::stoi(measure->GetN());
        }
        if (system) {
            endEntry->systemNo = system->GetSystemIdx();
        }
        if (page) {
            endEntry->pageNo = page->GetPageIdx();
        }
    }
    else if (object->Is(MEASURE)) {

        Measure *measure = vrv_cast<Measure *>(object);
        assert(measure);

        // Deal with repeated music later, for now get the last times.
        double scoreTimeStart = params->m_scoreTimeOffset;
        double realTimeStart = params->m_realTimeOffsetMilliseconds;

        if (m_map.count(realTimeStart) == 0) {
            m_map[realTimeStart] = emptyEntry;
        }
        TimemapEntry *startEntry = &m_map.at(realTimeStart);

        // Should check if value for realTimeStart already exists and if so, then
        // ensure that it is equal to scoreTimeStart:
        startEntry->qstamp = scoreTimeStart;

        // Add the measureOn
        startEntry->measureOn = measure->GetUuid();
        startEntry->measureNo = std::stoi(measure->GetN());

        System* system = dynamic_cast<System*>(object->GetFirstAncestor(SYSTEM));
        if (system) {
            startEntry->systemNo = system->GetSystemIdx();
        }
        Page* page = dynamic_cast<Page*>(object->GetFirstAncestor(PAGE));
        if (page) {
            startEntry->pageNo = page->GetPageIdx();
        }
    }
}

void Timemap::ToJson(std::string &output, bool includeRests, bool includeMeasures)
{
    double currentTempo = -1000.0;
    double newTempo;

    jsonxx::Array timemap;

    for (auto &[tstamp, entry] : m_map) {
        jsonxx::Object o;
        o << "tstamp" << tstamp;
        o << "qstamp" << entry.qstamp;

        // on / off
        if (!entry.notesOn.empty()) {
            jsonxx::Array notesOn;
            for (auto& [pitch, pitchEntry] : entry.notesOn) {
                jsonxx::Object item;
                item << "pitch" << pitch;

                item << "staff" << pitchEntry.first;

                jsonxx::Array ids;
                for (auto& id : pitchEntry.second) {
                    ids << id;
                }
                item << "ids" << ids;
                notesOn << item;
            }
            o << "on" << notesOn;
        }
        if (!entry.notesOff.empty()) {
            jsonxx::Array notesOff;
            for (auto& [pitch, pitchEntry] : entry.notesOff) {
                jsonxx::Object item;
                item << "pitch" << pitch;

                item << "staff" << pitchEntry.first;

                jsonxx::Array ids;
                for (auto& id : pitchEntry.second) {
                    ids << id;
                }
                item << "ids" << ids;
                notesOff << item;
            }
            o << "off" << notesOff;
        }

        // restsOn / restsOff
        if (includeRests) {
            if (!entry.restsOn.empty()) {
                jsonxx::Array restsOn;
                for (auto& rest : entry.restsOn) restsOn << rest;
                o << "restsOn" << restsOn;
            }
            if (!entry.restsOff.empty()) {
                jsonxx::Array restsOff;
                for (auto& rest : entry.restsOff) restsOff << rest;
                o << "restsOff" << restsOff;
            }
        }

        // tempo
        if (entry.tempo != -1000.0) {
            newTempo = entry.tempo;
            if (newTempo != currentTempo) {
                currentTempo = newTempo;
                o << "tempo" << std::to_string(currentTempo);
            }
        }

        // measureOn
        if (includeMeasures && !entry.measureOn.empty()) {
            o << "measureOn" << entry.measureOn;
        }
        o << "measureNo" << entry.measureNo;
        o << "systemNo" << entry.systemNo;
        o << "pageNo" << entry.pageNo;

        timemap << o;
    }
    output = timemap.json();
}

} // namespace vrv

/////////////////////////////////////////////////////////////////////////////
// Name:        midiext.h
// Author:      Herbert
// Created:     23/04/2022
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_MIDIEXT_H__
#define __VRV_MIDIEXT_H__

#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include <array>

//----------------------------------------------------------------------------

namespace vrv {

    class Object;

    class Note;

    class Measure;

//----------------------------------------------------------------------------
// TimemapEntry
//----------------------------------------------------------------------------

/**
 * Helper struct to store timemap entries
 */
    struct MidiExtEntry {
        std::map<int, std::pair<int, std::vector<std::string>>> notesOn;    // pitch:staff:ids
        std::map<int, std::pair<int, std::vector<std::string>>> notesOff;
        int measureNo;
        int pageNo;
    };

    struct MidiExtMeasure {
        int measureNo;
        int duration;
        int systemNo;
    };

//----------------------------------------------------------------------------
// Timemap
//----------------------------------------------------------------------------

/**
 * This class holds a timemap for exporting onset / offset values.
 */
    class MidiExt {
    public:
        /**
         * @name Constructors, destructors, and other standard methods
         */
        ///@{
        explicit MidiExt();

        virtual ~MidiExt();
        ///@}

        /** Resets the timemap */
        void Reset();

        void AddNote(int tick, Note *note);

        MidiExtEntry *GetTimeEntry(int tick);

        void AddMeasure(int tick, int duration, Measure *measure);

        void CopyMeasures(int fromTick, int endTick, int addTick);

        void CopyTimeEntry(int fromTick, int endTick, int addTick);

        const std::map<int, MidiExtMeasure> &GetMeasures() const;

        const std::map<int, MidiExtEntry> &GetEntries() const;

        const std::map<std::string, int> &GetSystems() const;

        const std::map<int, std::map<int, std::map<int, int>>> &GetAdjustedLayers() const;

    private:
        //
    public:
        //
    private:
        /** The map with time values as keys */
        std::map<int, MidiExtEntry> m_entries;
        std::map<int, MidiExtMeasure> m_measureTicks;
        std::map<std::string, int> m_systemUuid;
        std::map<int, std::map<int, std::map<int, int>>> m_adjustedLayers; // measure:staff:layer:staff
    }; // class Timemap

} // namespace vrv

#endif // __VRV_MIDIEXT_H__

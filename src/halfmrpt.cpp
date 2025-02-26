/////////////////////////////////////////////////////////////////////////////
// Name:        halfmrpt.cpp
// Author:      Laurent Pugin
// Created:     2018
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "halfmrpt.h"

//----------------------------------------------------------------------------

#include <cassert>
#include <math.h>

//----------------------------------------------------------------------------

#include "chord.h"
#include "editorial.h"
#include "functorparams.h"
#include "layer.h"
#include "note.h"
#include "staff.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// HalfmRpt
//----------------------------------------------------------------------------

static const ClassRegistrar<HalfmRpt> s_factory("halfmRpt", HALFMRPT);

HalfmRpt::HalfmRpt() : LayerElement(HALFMRPT, "mrpt-")
{
    this->RegisterAttClass(ATT_COLOR);

    this->Reset();
}

HalfmRpt::~HalfmRpt() {}

void HalfmRpt::Reset()
{
    LayerElement::Reset();
    this->ResetColor();
}

//----------------------------------------------------------------------------
// HalfmRpt functor methods
//----------------------------------------------------------------------------

int HalfmRpt::GenerateMIDI(FunctorParams *functorParams)
{
    // GenerateMIDIParams *params = vrv_params_cast<GenerateMIDIParams *>(functorParams);
    // assert(params);

    LogWarning("HalfmRpt produces empty MIDI output");

    return FUNCTOR_CONTINUE;
}

int HalfmRpt::PrepareRpt(FunctorParams *functorParams)
{
    // PrepareRptParams *params = vrv_params_cast<PrepareRptParams *>(functorParams);
    // assert(params);

    return FUNCTOR_CONTINUE;
}

} // namespace vrv

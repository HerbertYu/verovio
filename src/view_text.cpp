/////////////////////////////////////////////////////////////////////////////
// Name:        view_text.cpp
// Author:      Laurent Pugin
// Created:     2015
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "view.h"

//----------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <math.h>
#include <sstream>

//----------------------------------------------------------------------------

#include "devicecontext.h"
#include "doc.h"
#include "dynam.h"
#include "f.h"
#include "fb.h"
#include "fig.h"
#include "lb.h"
#include "num.h"
#include "options.h"
#include "rend.h"
#include "smufl.h"
#include "staff.h"
#include "svg.h"
#include "symbol.h"
#include "system.h"
#include "text.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// View - TextElement
//----------------------------------------------------------------------------

void View::DrawF(DeviceContext *dc, F *f, TextDrawingParams &params)
{
    assert(dc);
    assert(f);

    dc->StartTextGraphic(f, "", f->GetID());

    this->DrawTextChildren(dc, f, params);

    if (f->GetStart() && f->GetEnd()) {
        System *currentSystem = dynamic_cast<System *>(f->GetFirstAncestor(SYSTEM));
        // Postpone the drawing of the end of the system; this will call DrawFConnector
        if (currentSystem) {
            currentSystem->AddToDrawingList(f);
        }
    }

    dc->EndTextGraphic(f, this);
}

void View::DrawTextString(DeviceContext *dc, std::wstring str, TextDrawingParams &params)
{
    assert(dc);

    dc->DrawText(UTF16to8(str), str);
}

void View::DrawDynamString(DeviceContext *dc, std::wstring str, TextDrawingParams &params, Rend *rend)
{
    assert(dc);

    const bool singleGlyphs = m_doc->GetOptions()->m_dynamSingleGlyphs.GetValue();

    if (rend && rend->HasFontfam()) {
        this->DrawTextString(dc, str, params);
        return;
    }

    if (params.m_textEnclose != ENCLOSURE_NONE) {
        std::wstring open;
        switch (params.m_textEnclose) {
            case ENCLOSURE_paren: open.push_back(L'('); break;
            case ENCLOSURE_brack: open.push_back(L'['); break;
            default: break;
        }
        this->DrawTextString(dc, open, params);
    }

    ArrayOfStringDynamTypePairs tokens;
    if (Dynam::GetSymbolsInStr(str, tokens)) {
        int first = true;
        for (auto &token : tokens) {
            if (!first) {
                // this->DrawTextString(dc, L" ", params);
            }
            first = false;

            if (token.second) {
                std::wstring smuflStr = Dynam::GetSymbolStr(token.first, singleGlyphs);
                FontInfo vrvTxt;
                vrvTxt.SetFaceName("VerovioText");
                vrvTxt.SetStyle(FONTSTYLE_normal);
                dc->SetFont(&vrvTxt);
                this->DrawTextString(dc, smuflStr, params);
                dc->ResetFont();
            }
            else {
                this->DrawTextString(dc, token.first, params);
            }
        }
    }
    else {
        this->DrawTextString(dc, str, params);
    }

    if (params.m_textEnclose != ENCLOSURE_NONE) {
        std::wstring close;
        switch (params.m_textEnclose) {
            case ENCLOSURE_paren: close.push_back(L')'); break;
            case ENCLOSURE_brack: close.push_back(L']'); break;
            default: break;
        }
        this->DrawTextString(dc, close, params);
    }
}

void View::DrawHarmString(DeviceContext *dc, std::wstring str, TextDrawingParams &params)
{
    assert(dc);

    int toDcX = ToDeviceContextX(params.m_x);
    int toDcY = ToDeviceContextY(params.m_y);

    std::size_t prevPos = 0, pos;
    while ((pos = str.find_first_of(VRV_TEXT_HARM, prevPos)) != std::wstring::npos) {
        // If pos is > than the previous, it is the substring to extract
        if (pos > prevPos) {
            std::wstring substr = str.substr(prevPos, pos - prevPos);
            dc->DrawText(UTF16to8(substr), substr, toDcX, toDcY);
            // Once we have rendered the some text to not pass x / y anymore
            toDcX = VRV_UNSET;
            toDcY = VRV_UNSET;
        }

        // if it is the same or we still have space, it is the accidental
        if (pos == prevPos || pos < str.length()) {
            // Then the accidental
            std::wstring accid = str.substr(pos, 1);
            std::wstring smuflAccid;
            if (accid == L"\u266D") { // MUSIC FLAT SIGN
                smuflAccid.push_back(SMUFL_E260_accidentalFlat);
            }
            else if (accid == L"\u266E") { // MUSIC NATURAL SIGN
                smuflAccid.push_back(SMUFL_E261_accidentalNatural);
            }
            else if (accid == L"\u266F") { // MUSIC SHARP SIGN
                smuflAccid.push_back(SMUFL_E262_accidentalSharp);
            }
            else {
                smuflAccid += accid;
            }

            FontInfo vrvTxt;
            vrvTxt.SetFaceName("VerovioText");
            dc->SetFont(&vrvTxt);
            // Once we have rendered the some text to not pass x / y anymore
            dc->DrawText(UTF16to8(smuflAccid), smuflAccid, toDcX, toDcY);
            dc->ResetFont();
            toDcX = VRV_UNSET;
            toDcY = VRV_UNSET;
        }
        // Skip the accidental and continue
        prevPos = pos + 1;
    }
    // Print the remainder of the string, or the full string if no accid
    if (prevPos < str.length()) {
        std::wstring substr = str.substr(prevPos, std::wstring::npos);
        dc->DrawText(UTF16to8(substr), substr, toDcX, toDcY);
    }

    // Disable x for what is comming next as child of <f>
    // The value is reset in DrawFb
    params.m_x = VRV_UNSET;
}

void View::DrawTextElement(DeviceContext *dc, TextElement *element, TextDrawingParams &params)
{
    assert(dc);
    assert(element);

    if (element->Is(FIGURE)) {
        F *f = vrv_cast<F *>(element);
        assert(f);
        this->DrawF(dc, f, params);
    }
    else if (element->Is(LB)) {
        Lb *lb = vrv_cast<Lb *>(element);
        assert(lb);
        this->DrawLb(dc, lb, params);
    }
    else if (element->Is(NUM)) {
        Num *num = vrv_cast<Num *>(element);
        assert(num);
        this->DrawNum(dc, num, params);
    }
    else if (element->Is(REND)) {
        Rend *rend = vrv_cast<Rend *>(element);
        assert(rend);
        this->DrawRend(dc, rend, params);
    }
    else if (element->Is(TEXT)) {
        Text *text = vrv_cast<Text *>(element);
        assert(text);
        this->DrawText(dc, text, params);
    }
    else {
        assert(false);
    }
}

void View::DrawLyricString(DeviceContext *dc, std::wstring str, int staffSize, std::optional<TextDrawingParams> params)
{
    assert(dc);

    bool wroteText = false;
    std::wistringstream iss(str);
    std::wstring token;
    while (std::getline(iss, token, L'_')) {
        wroteText = true;
        if (params) {
            dc->DrawText(UTF16to8(token), token, params->m_x, params->m_y, params->m_width, params->m_height);
        }
        else {
            dc->DrawText(UTF16to8(token), token);
        }

        // no _
        if (iss.eof()) break;

        FontInfo vrvTxt;
        vrvTxt.SetFaceName("VerovioText");
        dc->SetFont(&vrvTxt);
        std::wstring str;
        str.push_back(VRV_TEXT_E551);
        if (params) {
            dc->DrawText(UTF16to8(str), str, params->m_x, params->m_y, params->m_width, params->m_height);
        }
        else {
            dc->DrawText(UTF16to8(str), str);
        }
        dc->ResetFont();
    }

    // This should only be called in facsimile mode where a zone is specified but there is
    // no text. This draws the bounds of the zone but leaves the space blank.
    if (!wroteText && params) {
        dc->DrawText("", L"", params->m_x, params->m_y, params->m_width, params->m_height);
    }
}

void View::DrawLb(DeviceContext *dc, Lb *lb, TextDrawingParams &params)
{
    assert(dc);
    assert(lb);

    dc->StartTextGraphic(lb, "", lb->GetID());

    FontInfo *currentFont = dc->GetFont();

    params.m_y -= m_doc->GetTextLineHeight(currentFont, false);
    params.m_explicitPosition = true;

    dc->EndTextGraphic(lb, this);
}

void View::DrawNum(DeviceContext *dc, Num *num, TextDrawingParams &params)
{
    assert(dc);
    assert(num);

    dc->StartTextGraphic(num, "", num->GetID());

    Text *currentText = num->GetCurrentText();
    if (currentText && (currentText->GetText().length() > 0)) {
        this->DrawText(dc, num->GetCurrentText(), params);
    }
    else {
        this->DrawTextChildren(dc, num, params);
    }

    dc->EndTextGraphic(num, this);
}

void View::DrawFig(DeviceContext *dc, Fig *fig, TextDrawingParams &params)
{
    assert(dc);
    assert(fig);

    dc->StartGraphic(fig, "", fig->GetID());

    Svg *svg = dynamic_cast<Svg *>(fig->FindDescendantByType(SVG));
    if (svg) {
        params.m_x = fig->GetDrawingX();
        params.m_y = fig->GetDrawingY();
        this->DrawSvg(dc, svg, params);
    }

    dc->EndGraphic(fig, this);
}

void View::DrawRend(DeviceContext *dc, Rend *rend, TextDrawingParams &params)
{
    assert(dc);
    assert(rend);

    dc->StartTextGraphic(rend, "", rend->GetID());

    if (params.m_laidOut) {
        if (params.m_alignment == HORIZONTALALIGNMENT_NONE) {
            params.m_alignment = rend->HasHalign() ? rend->GetHalign() : HORIZONTALALIGNMENT_left;
            params.m_x = rend->GetDrawingX();
            params.m_y = rend->GetDrawingY();
            dc->MoveTextTo(ToDeviceContextX(params.m_x), ToDeviceContextY(params.m_y), params.m_alignment);
        }
    }

    FontInfo rendFont;
    bool customFont = false;
    if (rend->HasFontname() || rend->HasFontsize() || rend->HasFontstyle() || rend->HasFontweight()) {
        customFont = true;
        if (rend->HasFontname()) rendFont.SetFaceName(rend->GetFontname().c_str());
        if (rend->HasFontsize()) {
            data_FONTSIZE *fs = rend->GetFontsizeAlternate();
            if (fs->GetType() == FONTSIZE_fontSizeNumeric) {
                rendFont.SetPointSize(fs->GetFontSizeNumeric());
            }
            else if (fs->GetType() == FONTSIZE_term) {
                const int percent = fs->GetPercentForTerm();
                rendFont.SetPointSize(params.m_pointSize * percent / 100);
            }
            else if (fs->GetType() == FONTSIZE_percent) {
                rendFont.SetPointSize(params.m_pointSize * fs->GetPercent() / 100);
            }
        }
        if (rend->HasFontstyle()) rendFont.SetStyle(rend->GetFontstyle());
        if (rend->HasFontweight()) rendFont.SetWeight(rend->GetFontweight());
    }

    if (customFont) dc->SetFont(&rendFont);

    int yShift = 0;
    if ((rend->GetRend() == TEXTRENDITION_sup) || (rend->GetRend() == TEXTRENDITION_sub)) {
        assert(dc->GetFont());
        int MHeight = m_doc->GetTextGlyphHeight('M', dc->GetFont(), false);
        if (rend->GetRend() == TEXTRENDITION_sup) {
            yShift += m_doc->GetTextGlyphHeight('o', dc->GetFont(), false);
            yShift += (MHeight * SUPER_SCRIPT_POSITION);
        }
        else {
            yShift += MHeight * SUB_SCRIPT_POSITION;
        }
        params.m_y += yShift;
        params.m_verticalShift = true;
        dc->GetFont()->SetSupSubScript(true);
        dc->GetFont()->SetPointSize(dc->GetFont()->GetPointSize() * SUPER_SCRIPT_FACTOR);
    }

    if ((rend->GetRend() == TEXTRENDITION_box) && (params.m_actualWidth != 0)) {
        params.m_x = params.m_actualWidth + m_doc->GetDrawingUnit(100);
        params.m_explicitPosition = true;
    }

    this->DrawTextChildren(dc, rend, params);

    if ((rend->GetRend() == TEXTRENDITION_sup) || (rend->GetRend() == TEXTRENDITION_sub)) {
        params.m_y -= yShift;
        params.m_verticalShift = true;
        dc->GetFont()->SetSupSubScript(false);
        dc->GetFont()->SetPointSize(dc->GetFont()->GetPointSize() / SUPER_SCRIPT_FACTOR);
    }

    if ((rend->GetRend() == TEXTRENDITION_box) || (rend->GetRend() == TEXTRENDITION_circle)) {
        params.m_enclosedRend.push_back(rend);
        params.m_x = rend->GetContentRight() + m_doc->GetDrawingUnit(100);
        params.m_explicitPosition = true;
        params.m_enclose = rend->GetRend();
    }

    if (customFont) dc->ResetFont();

    dc->EndTextGraphic(rend, this);
}

void View::DrawText(DeviceContext *dc, Text *text, TextDrawingParams &params)
{
    assert(dc);
    assert(text);

    const Resources *resources = dc->GetResources();
    assert(resources);

    dc->StartTextGraphic(text, "", text->GetID());

    resources->SelectTextFont(dc->GetFont()->GetWeight(), dc->GetFont()->GetStyle());

    if (params.m_explicitPosition) {
        dc->MoveTextTo(ToDeviceContextX(params.m_x), ToDeviceContextY(params.m_y), HORIZONTALALIGNMENT_NONE);
        params.m_explicitPosition = false;
    }
    else if (params.m_verticalShift) {
        dc->MoveTextVerticallyTo(ToDeviceContextY(params.m_y));
        params.m_verticalShift = false;
    }

    // special case where we want to replace the '#' or 'b' with a VerovioText glyphs
    if (text->GetFirstAncestor(DYNAM)) {
        this->DrawDynamString(dc, text->GetText(), params, dynamic_cast<Rend *>(text->GetFirstAncestor(REND)));
    }
    else if (text->GetFirstAncestor(HARM)) {
        this->DrawHarmString(dc, text->GetText(), params);
    }
    // special case where we want to replace the '_' with a lyric connector
    // '_' are produce with the SibMEI plugin
    else if (text->GetFirstAncestor(SYL)) {
        if (params.m_height != VRV_UNSET && params.m_height != 0) {
            this->DrawLyricString(dc, text->GetText(), 100, params);
        }
        else {
            this->DrawLyricString(dc, text->GetText());
        }
    }
    else {
        this->DrawTextString(dc, text->GetText(), params);
    }

    params.m_actualWidth = text->GetContentRight();

    dc->EndTextGraphic(text, this);
}

void View::DrawSvg(DeviceContext *dc, Svg *svg, TextDrawingParams &params)
{
    assert(dc);
    assert(svg);

    dc->StartGraphic(svg, "", svg->GetID());

    dc->DrawSvgShape(
        ToDeviceContextX(params.m_x), ToDeviceContextY(params.m_y), svg->GetWidth(), svg->GetHeight(), svg->Get());

    dc->EndGraphic(svg, this);
}

void View::DrawSymbol(DeviceContext *dc, Staff *staff, Symbol *symbol, TextDrawingParams &params)
{
    assert(dc);
    assert(symbol);

    dc->StartGraphic(symbol, "", symbol->GetID());

    const wchar_t code = symbol->GetSymbolGlyph();

    this->DrawSmuflCode(dc, params.m_x, params.m_y, code, staff->m_drawingStaffSize, false);

    if (code) {
        params.m_x += m_doc->GetGlyphAdvX(code, staff->m_drawingStaffSize, false);
    }

    dc->EndGraphic(symbol, this);
}

} // namespace vrv

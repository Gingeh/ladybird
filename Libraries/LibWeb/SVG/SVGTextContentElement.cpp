/*
 * Copyright (c) 2022, Andreas Kling <andreas@ladybird.org>
 * Copyright (c) 2023, Aliaksandr Kalenik <kalenik.aliaksandr@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/SVGTextContentElementPrototype.h>
#include <LibWeb/DOM/Document.h>
#include <LibWeb/Layout/Node.h>
#include <LibWeb/SVG/AttributeParser.h>
#include <LibWeb/SVG/SVGTextContentElement.h>

namespace Web::SVG {

SVGTextContentElement::SVGTextContentElement(DOM::Document& document, DOM::QualifiedName qualified_name)
    : SVGGraphicsElement(document, move(qualified_name))
{
}

void SVGTextContentElement::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(SVGTextContentElement);
    Base::initialize(realm);
}

Optional<TextAnchor> SVGTextContentElement::text_anchor() const
{
    if (!layout_node())
        return {};
    switch (layout_node()->computed_values().text_anchor()) {
    case CSS::TextAnchor::Start:
        return TextAnchor::Start;
    case CSS::TextAnchor::Middle:
        return TextAnchor::Middle;
    case CSS::TextAnchor::End:
        return TextAnchor::End;
    default:
        VERIFY_NOT_REACHED();
    }
}

ByteString SVGTextContentElement::text_contents() const
{
    return child_text_content().to_byte_string().trim_whitespace();
}

// https://svgwg.org/svg2-draft/text.html#__svg__SVGTextContentElement__getNumberOfChars
WebIDL::ExceptionOr<WebIDL::Long> SVGTextContentElement::get_number_of_chars() const
{
    auto length_in_code_units = AK::utf16_code_unit_length_from_utf8(text_contents());
    return static_cast<WebIDL::Long>(length_in_code_units);
}

GC::Ref<Geometry::DOMPoint> SVGTextContentElement::get_start_position_of_char(WebIDL::UnsignedLong charnum)
{
    dbgln("(STUBBED) SVGTextContentElement::get_start_position_of_char(charnum={}). Called on: {}", charnum, debug_description());
    return Geometry::DOMPoint::from_point(vm(), Geometry::DOMPointInit {});
}

}

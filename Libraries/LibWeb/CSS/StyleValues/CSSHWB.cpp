/*
 * Copyright (c) 2024, Sam Atkins <sam@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "CSSHWB.h"
#include <AK/TypeCasts.h>
#include <LibWeb/CSS/Serialize.h>
#include <LibWeb/CSS/StyleValues/CalculatedStyleValue.h>
#include <LibWeb/CSS/StyleValues/PercentageStyleValue.h>
#include <LibWeb/Layout/Node.h>

namespace Web::CSS {

Optional<Color> CSSHWB::to_color(Optional<Layout::NodeWithStyle const&> node, CalculationResolutionContext const& resolution_context) const
{
    auto component_resolution_context = resolution_context;
    if (m_properties.origin) {
        auto resolved_origin = m_properties.origin->to_color(node, resolution_context);
        if (!resolved_origin.has_value())
            return {};

        double r = resolved_origin->red() / 255.0;
        double g = resolved_origin->green() / 255.0;
        double b = resolved_origin->blue() / 255.0;

        double maxVal = max(r, max(g, b));
        double minVal = min(r, min(g, b));
        double delta = maxVal - minVal;

        double hue;
        if (delta == 0) {
            hue = 0.0; // undefined hue for gray; conventionally set to 0
        } else if (maxVal == r) {
            hue = 60.0 * fmod(((g - b) / delta), 6.0);
        } else if (maxVal == g) {
            hue = 60.0 * (((b - r) / delta) + 2.0);
        } else { // maxVal == b
            hue = 60.0 * (((r - g) / delta) + 4.0);
        }
        if (hue < 0.0)
            hue += 360.0;

        double whiteness = minVal * 100.0;
        double blackness = (1.0 - maxVal) * 100.0;

        component_resolution_context.resolved_keyword_values.set(Keyword::H, hue);
        component_resolution_context.resolved_keyword_values.set(Keyword::W, whiteness);
        component_resolution_context.resolved_keyword_values.set(Keyword::B, blackness);
        component_resolution_context.resolved_keyword_values.set(Keyword::Alpha, ((double)resolved_origin->alpha()) / 255.0);
    }

    auto h_val = resolve_hue(m_properties.h, component_resolution_context);
    auto raw_w_value = resolve_with_reference_value(m_properties.w, 100.0, component_resolution_context);
    auto raw_b_value = resolve_with_reference_value(m_properties.b, 100.0, component_resolution_context);
    auto alpha_val = resolve_alpha(m_properties.alpha, component_resolution_context);

    if (!h_val.has_value() || !raw_w_value.has_value() || !raw_b_value.has_value() || !alpha_val.has_value())
        return {};

    auto w_val = clamp(raw_w_value.value(), 0, 100) / 100.0f;
    auto b_val = clamp(raw_b_value.value(), 0, 100) / 100.0f;

    if (w_val + b_val >= 1.0f) {
        auto to_byte = [](float value) {
            return round_to<u8>(clamp(value * 255.0f, 0.0f, 255.0f));
        };
        u8 gray = to_byte(w_val / (w_val + b_val));
        return Color(gray, gray, gray, to_byte(alpha_val.value()));
    }

    auto value = 1 - b_val;
    auto saturation = 1 - (w_val / value);
    return Color::from_hsv(h_val.value(), saturation, value).with_opacity(alpha_val.value());
}

bool CSSHWB::equals(CSSStyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& other_color = other.as_color();
    if (color_type() != other_color.color_type())
        return false;
    auto const& other_hwb = as<CSSHWB>(other_color);
    return m_properties == other_hwb.m_properties;
}

// https://www.w3.org/TR/css-color-4/#serializing-sRGB-values
String CSSHWB::to_string(SerializationMode mode) const
{
    if (mode == SerializationMode::Normal && m_properties.origin) {
        StringBuilder builder;
        builder.append("hwb(from "sv);
        builder.append(m_properties.origin->to_string(mode));
        builder.append(" "sv);
        builder.append(m_properties.h->to_string(mode));
        builder.append(" "sv);
        builder.append(m_properties.w->to_string(mode));
        builder.append(" "sv);
        builder.append(m_properties.b->to_string(mode));
        if ((!m_properties.alpha->is_number() || m_properties.alpha->as_number().number() < 1) && (!m_properties.alpha->is_percentage() || m_properties.alpha->as_percentage().percentage().as_fraction() < 1)) {
            builder.append(" / "sv);
            builder.append(m_properties.alpha->to_string(mode));
        }
        builder.append(")"sv);

        return builder.to_string_without_validation();
    }

    if (auto color = to_color({}, {}); color.has_value())
        return serialize_a_srgb_value(color.value());

    StringBuilder builder;
    builder.append("hwb("sv);
    serialize_hue_component(builder, mode, m_properties.h);
    builder.append(" "sv);
    serialize_color_component(builder, mode, m_properties.w, 100, 0);
    builder.append(" "sv);
    serialize_color_component(builder, mode, m_properties.b, 100, 0);
    if ((!m_properties.alpha->is_number() || m_properties.alpha->as_number().number() < 1) && (!m_properties.alpha->is_percentage() || m_properties.alpha->as_percentage().percentage().as_fraction() < 1)) {
        builder.append(" / "sv);
        serialize_alpha_component(builder, mode, m_properties.alpha);
    }
    builder.append(")"sv);

    return builder.to_string_without_validation();
}

}

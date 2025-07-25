/*
 * Copyright (c) 2020-2022, Andreas Kling <andreas@ladybird.org>
 * Copyright (c) 2021-2022, Linus Groh <linusg@serenityos.org>
 * Copyright (c) 2022, Sam Atkins <atkinssj@serenityos.org>
 * Copyright (c) 2023, MacDue <macdue@dueutil.tech>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibWeb/CSS/Parser/Parser.h>
#include <LibWeb/HTML/Canvas/CanvasState.h>
#include <LibWeb/HTML/CanvasGradient.h>
#include <LibWeb/HTML/CanvasPattern.h>
#include <LibWeb/Layout/Node.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/canvas.html#canvasfillstrokestyles
template<typename IncludingClass>
class CanvasFillStrokeStyles {
public:
    ~CanvasFillStrokeStyles() = default;
    using FillOrStrokeStyleVariant = Variant<String, GC::Root<CanvasGradient>, GC::Root<CanvasPattern>>;

    void set_fill_style(FillOrStrokeStyleVariant style)
    {
        // https://html.spec.whatwg.org/multipage/canvas.html#dom-context-2d-fillstyle
        style.visit(
            // 1. If the given value is a string, then:
            [&](String const& string) {
                // 1. Let context be this's canvas attribute's value, if that is an element; otherwise null.
                HTMLCanvasElement* context = my_canvas_element().visit(
                    [&](HTMLCanvasElement* canvas_element) -> HTMLCanvasElement* {
                        return canvas_element;
                    },
                    [&](OffscreenCanvas*) -> HTMLCanvasElement* {
                        return nullptr;
                    });

                // 2. Let parsedValue be the result of parsing the given value with context if non-null.
                // FIXME: Parse a color value
                // https://drafts.csswg.org/css-color/#parse-a-css-color-value
                auto style_value = parse_css_value(CSS::Parser::ParsingParams(), string, CSS::PropertyID::Color);
                if (style_value && style_value->has_color()) {
                    Optional<Layout::NodeWithStyle const&> layout_node;
                    CSS::CalculationResolutionContext resolution_context {};
                    if (context && context->layout_node()) {
                        layout_node = *context->layout_node();
                        resolution_context.length_resolution_context = CSS::Length::ResolutionContext::for_layout_node(*context->layout_node());
                    }

                    auto parsedValue = style_value->to_color(layout_node, resolution_context).value_or(Color::Black);

                    // 4. Set this's fill style to parsedValue.
                    my_drawing_state().fill_style = parsedValue;
                } else {
                    // 3. If parsedValue is failure, then return.
                    return;
                }

                // 5. Return.
                return;
            },
            [&](auto fill_or_stroke_style) {
                // FIXME: 2. If the given value is a CanvasPattern object that is marked as not origin-clean, then set this's origin-clean flag to false.

                // 3. Set this's fill style to the given value.
                my_drawing_state().fill_style = fill_or_stroke_style;
            });
    }

    FillOrStrokeStyleVariant fill_style() const
    {
        return my_drawing_state().fill_style.to_js_fill_or_stroke_style();
    }

    void set_stroke_style(FillOrStrokeStyleVariant style)
    {
        // https://html.spec.whatwg.org/multipage/canvas.html#dom-context-2d-strokestyle

        style.visit(
            // 1. If the given value is a string, then:
            [&](String const& string) {
                // 1. Let context be this's canvas attribute's value, if that is an element; otherwise null.
                HTMLCanvasElement* context = my_canvas_element().visit(
                    [&](HTMLCanvasElement* canvas_element) -> HTMLCanvasElement* {
                        return canvas_element;
                    },
                    [&](OffscreenCanvas*) -> HTMLCanvasElement* {
                        return nullptr;
                    });

                // 2. Let parsedValue be the result of parsing the given value with context if non-null.
                // FIXME: Parse a color value
                // https://drafts.csswg.org/css-color/#parse-a-css-color-value
                auto style_value = parse_css_value(CSS::Parser::ParsingParams(), string, CSS::PropertyID::Color);
                if (style_value && style_value->has_color()) {
                    Optional<Layout::NodeWithStyle const&> layout_node;
                    CSS::CalculationResolutionContext resolution_context {};
                    if (context && context->layout_node()) {
                        layout_node = *context->layout_node();
                        resolution_context.length_resolution_context = CSS::Length::ResolutionContext::for_layout_node(*context->layout_node());
                    }

                    auto parsedValue = style_value->to_color(layout_node, resolution_context).value_or(Color::Black);

                    // 4. Set this's stroke style to parsedValue.
                    my_drawing_state().stroke_style = parsedValue;
                } else {
                    // 3. If parsedValue is failure, then return.
                    return;
                }

                // 5. Return.
                return;
            },
            [&](auto fill_or_stroke_style) {
                // FIXME: 2. If the given value is a CanvasPattern object that is marked as not origin-clean, then set this's origin-clean flag to false.

                // 3. Set this's stroke style to the given value.
                my_drawing_state().fill_style = fill_or_stroke_style;
            });
    }

    FillOrStrokeStyleVariant stroke_style() const
    {
        return my_drawing_state().stroke_style.to_js_fill_or_stroke_style();
    }

    WebIDL::ExceptionOr<GC::Ref<CanvasGradient>> create_radial_gradient(double x0, double y0, double r0, double x1, double y1, double r1)
    {
        auto& realm = static_cast<IncludingClass&>(*this).realm();
        return CanvasGradient::create_radial(realm, x0, y0, r0, x1, y1, r1);
    }

    GC::Ref<CanvasGradient> create_linear_gradient(double x0, double y0, double x1, double y1)
    {
        auto& realm = static_cast<IncludingClass&>(*this).realm();
        return CanvasGradient::create_linear(realm, x0, y0, x1, y1).release_value_but_fixme_should_propagate_errors();
    }

    GC::Ref<CanvasGradient> create_conic_gradient(double start_angle, double x, double y)
    {
        auto& realm = static_cast<IncludingClass&>(*this).realm();
        return CanvasGradient::create_conic(realm, start_angle, x, y).release_value_but_fixme_should_propagate_errors();
    }

    WebIDL::ExceptionOr<GC::Ptr<CanvasPattern>> create_pattern(CanvasImageSource const& image, StringView repetition)
    {
        auto& realm = static_cast<IncludingClass&>(*this).realm();
        return CanvasPattern::create(realm, image, repetition);
    }

protected:
    CanvasFillStrokeStyles() = default;

private:
    Variant<HTMLCanvasElement*, OffscreenCanvas*> my_canvas_element() { return &reinterpret_cast<IncludingClass&>(*this).canvas_element(); }
    CanvasState::DrawingState& my_drawing_state() { return reinterpret_cast<IncludingClass&>(*this).drawing_state(); }
    CanvasState::DrawingState const& my_drawing_state() const { return reinterpret_cast<IncludingClass const&>(*this).drawing_state(); }
};

}

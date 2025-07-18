/*
 * Copyright (c) 2021-2025, Tim Flynn <trflynn89@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Runtime/Intl/DateTimeFormat.h>
#include <LibJS/Runtime/PrototypeObject.h>

namespace JS::Intl {

class JS_API DateTimeFormatPrototype final : public PrototypeObject<DateTimeFormatPrototype, DateTimeFormat> {
    JS_PROTOTYPE_OBJECT(DateTimeFormatPrototype, DateTimeFormat, Intl.DateTimeFormat);
    GC_DECLARE_ALLOCATOR(DateTimeFormatPrototype);

public:
    virtual void initialize(Realm&) override;
    virtual ~DateTimeFormatPrototype() override = default;

private:
    explicit DateTimeFormatPrototype(Realm&);

    JS_DECLARE_NATIVE_FUNCTION(resolved_options);
    JS_DECLARE_NATIVE_FUNCTION(format);
    JS_DECLARE_NATIVE_FUNCTION(format_range);
    JS_DECLARE_NATIVE_FUNCTION(format_range_to_parts);
    JS_DECLARE_NATIVE_FUNCTION(format_to_parts);
};

}

#!/usr/bin/env swift
/*
 This source file is part of the Swift.org open source project

 Copyright (c) 2025 Apple Inc. and the Swift project authors
 Licensed under Apache License v2.0 with Runtime Library Exception

 See https://swift.org/LICENSE.txt for license information
 See https://swift.org/CONTRIBUTORS.txt for Swift project authors
*/

// Generates a list of CJK character ranges for the purpose of emphasis delimiter calculation.
// Adapted from `cjk-ranges.ts` in https://github.com/tats-u/markdown-cjk-friendly whose CommonMark
// specification amendments were used to improve CJK text handling.

import Foundation
import RegexBuilder

/// The Unicode version used for the following data files. Printed into the generated code file.
///
/// > Important: If you update these data files, please change this string and the URLs below.
/// > Also check the linked section for `unassignedAsCjkRanges` to see if that needs updating.
let unicodeVersion = "16.0"

let dataDirectory = URL(fileURLWithPath: #file)
    .deletingLastPathComponent().deletingLastPathComponent()
    .appending(components: "data")

// https://www.unicode.org/reports/tr41/tr41-34.html#Data11
// Local copy fetched from https://www.unicode.org/Public/16.0.0/ucd/EastAsianWidth.txt
let eawInputDataURL =
    dataDirectory.appending(components: "EastAsianWidth.txt")

// https://www.unicode.org/reports/tr41/tr41-34.html#Data24
// Local copy fetched from https://www.unicode.org/Public/16.0.0/ucd/Scripts.txt
let scriptsInputDataURL =
    dataDirectory.appending(components: "Scripts.txt")

// https://www.unicode.org/reports/tr51/#emoji_data
// Local copy fetched from https://www.unicode.org/Public/emoji/16.0/emoji-sequences.txt
let emojiSequencesInputDataURL =
    dataDirectory.appending(components: "emoji-sequences.txt")

// Same source as above
// Local copy fetched from https://www.unicode.org/Public/16.0.0/ucd/emoji/emoji-variation-sequences.txt
let emojiVariationSequencesInputDataURL =
    dataDirectory.appending(components: "emoji-variation-sequences.txt")

extension UInt32 {
    var hexString: String {
        String(self, radix: 16, uppercase: true)
    }
}

enum CodePointRange: Hashable {
    case single(UInt32)
    case range(ClosedRange<UInt32>)

    init(begin: UInt32, end: UInt32?) {
        if let end {
            self = .range(begin...end)
        } else {
            self = .single(begin)
        }
    }

    /// The equivalent C expression for this range of code points when comparing to a value named `uc`.
    var cExpression: String {
        switch self {
        case let .single(codePoint):
            return "(uc == 0x\(codePoint.hexString))"
        case let .range(range):
            return "(0x\(range.lowerBound.hexString) <= uc && uc <= 0x\(range.upperBound.hexString))"
        }
    }
}

extension [CodePointRange] {
    func forEachCodePoint(transform: (UInt32) -> Void) {
        for range in self {
            switch range {
            case let .single(codePoint):
                transform(codePoint)
            case let .range(range):
                range.forEach(transform)
            }
        }
    }
}

/// Returns the ranges of code points that have an East Asian Width of W, F, or H.
func cjkEawRanges() async throws -> (cjkRanges: [CodePointRange], nonCjkRanges: [CodePointRange]) {
    let codePointRegex = Repeat(4...5) {
        One(.hexDigit)
    }

    let codePointRef = Reference(UInt32.self)
    let codePointEndRef = Reference(UInt32?.self)
    let widthTypeRef = Reference(String.self)
    let lineMatchRegex = Regex {
        Anchor.startOfLine
        TryCapture(as: codePointRef) {
            codePointRegex
        } transform: { UInt32($0, radix: 16) }
        Optionally {
            ".."
            Capture(as: codePointEndRef) {
                codePointRegex
            } transform: { UInt32($0, radix: 16) }
        }
        OneOrMore(.whitespace)
        "; "
        Capture(as: widthTypeRef) {
            // The East Asian Width value abbreviations, as defined here:
            // https://www.unicode.org/reports/tr11/#Definitions
            ChoiceOf {
                "A"
                "F"
                "H"
                "N"
                "Na"
                "W"
            }
        } transform: {
            $0.uppercased()
        }
        OneOrMore(.whitespace)
        "#"
    }

    var cjkRanges: [CodePointRange] = []
    var nonCjkRanges: [CodePointRange] = []
    for try await line in eawInputDataURL.lines {
        if let match = try lineMatchRegex.firstMatch(in: line) {
            let range: CodePointRange
            if let codePointEnd = match[codePointEndRef] {
                range = .range(match[codePointRef]...codePointEnd)
            } else {
                range = .single(match[codePointRef])
            }

            // Wide, Fullwidth and Halfwidth values are considered CJK for this segment
            if ["W", "F", "H"].contains(match[widthTypeRef]) {
                cjkRanges.append(range)
            } else {
                nonCjkRanges.append(range)
            }
        }
    }

    return (cjkRanges, nonCjkRanges)
}

/// Returns the ranges of code points with a Script property of Hangul.
func hangulRanges() async throws -> [CodePointRange] {
    let codePointRegex = Repeat(4...5) {
        One(.hexDigit)
    }

    let codePointRef = Reference(UInt32.self)
    let codePointEndRef = Reference(UInt32?.self)
    let lineMatchRegex = Regex {
        Anchor.startOfLine
        TryCapture(as: codePointRef) {
            codePointRegex
        } transform: { UInt32($0, radix: 16) }
        Optionally {
            ".."
            Capture(as: codePointEndRef) {
                codePointRegex
            } transform: { UInt32($0, radix: 16) }
        }
        OneOrMore(.whitespace)
        "; "
        "Hangul"
        OneOrMore(.whitespace)
        "#"
    }

    var ranges: [CodePointRange] = []
    for try await line in scriptsInputDataURL.lines {
        if let match = try lineMatchRegex.firstMatch(in: line) {
            if let codePointEnd = match[codePointEndRef] {
                ranges.append(.range(match[codePointRef]...codePointEnd))
            } else {
                ranges.append(.single(match[codePointRef]))
            }
        }
    }
    return ranges
}

/// Returns the ranges of single code points that count as emoji.
func singleCodePointEmojiRanges() async throws -> [CodePointRange] {
    let codePointRegex = Repeat(4...5) {
        One(.hexDigit)
    }

    let codePointRef = Reference(UInt32.self)
    let codePointEndRef = Reference(UInt32?.self)

    // This regex captures single codepoints or codepoint ranges, but not codepoint
    // sequences like `XXXX FE0F`
    let lineMatchRegex = Regex {
        Anchor.startOfLine
        TryCapture(as: codePointRef) {
            codePointRegex
        } transform: { UInt32($0, radix: 16) }
        Optionally {
            ".."
            Capture(as: codePointEndRef) {
                codePointRegex
            } transform: { UInt32($0, radix: 16) }
        }
        OneOrMore(.whitespace)
        ";"
    }

    var ranges: [CodePointRange] = []
    for try await line in emojiSequencesInputDataURL.lines {
        if let match = try lineMatchRegex.firstMatch(in: line) {
            if let codePointEnd = match[codePointEndRef] {
                ranges.append(.range(match[codePointRef]...codePointEnd))
            } else {
                ranges.append(.single(match[codePointRef]))
            }
        }
    }
    return ranges
}

/// Ranges of code points referenced in `EastAsianWidth.txt` that default to `W` if not otherwise stated in that file.
///
/// Code points that are not mentioned in the file nor in these defined ranges would otherwise
/// default to `N`.
///
/// This diverges slightly from the code in `cjk-ranges.ts` and the comments in
/// `EastAsianWidth.txt` to reflect the standards doc itself:
/// https://www.unicode.org/reports/tr11/#Unassigned
/// Specifically, this uses the full ranges of the Ideographic Planes, instead of the truncated
/// ranges mentioned in the comment of `EastAsianWidth.txt`.
let unassignedAsCjkRanges: [CodePointRange] = [
    .range(0x3400...0x4DBF), // CJK Unified Ideographs Extension A
    .range(0x4E00...0x9FFF), // CJK Unified Ideographs
    .range(0xF900...0xFAFF), // CJK Compatibility Ideographs
    .range(0x20000...0x2FFFF), // Supplementary Ideographic Plane
    .range(0x30000...0x3FFFF), // Tertiary Ideographic Plane
]

/// Returns the emoji code points that have a selectable text representation.
func textSwitchableEmojiRanges() async throws -> [CodePointRange] {
    let codePointRegex = Repeat(4...5) {
        One(.hexDigit)
    }

    let codePointRef = Reference(UInt32.self)
    let lineMatchRegex = Regex {
        Anchor.startOfLine
        TryCapture(as: codePointRef) {
            codePointRegex
        } transform: { UInt32($0, radix: 16) }
        " FE0E"
        OneOrMore(.whitespace)
        ";"
    }

    var ranges: [CodePointRange] = []
    for try await line in emojiVariationSequencesInputDataURL.lines {
        if let match = try lineMatchRegex.firstMatch(in: line) {
            ranges.append(.single(match[codePointRef]))
        }
    }
    return ranges
}

/// Returns the non-emoji code points that have a selectable emoji representation.
func emojiSwitchableTextRanges() async throws -> [CodePointRange] {
    let codePointRegex = Repeat(4...5) {
        One(.hexDigit)
    }

    let codePointRef = Reference(UInt32.self)
    let lineMatchRegex = Regex {
        Anchor.startOfLine
        TryCapture(as: codePointRef) {
            codePointRegex
        } transform: { UInt32($0, radix: 16) }
        " FE0F"
        OneOrMore(.whitespace)
        ";"
    }

    var ranges: [CodePointRange] = []
    for try await line in emojiVariationSequencesInputDataURL.lines {
        if let match = try lineMatchRegex.firstMatch(in: line) {
            ranges.append(.single(match[codePointRef]))
        }
    }
    return ranges
}

/// Generates a mapping of codepoints to whether or not that codepoint is considered CJK for the
/// purpose of emphasis delimiters.
///
/// A code point is considered CJK if either of these are true:
/// - The Script property of the code point is Hangul, or
/// - Both of the following:
///   - The East Asian Width property of the code point is `W`, `F`, or `H`, and
///   - The code point is not a fully-qualified emoji.
///
/// For the purposes of this script, only single-code-point emoji are considered. Other code points
/// that can start or end emoji are handled outside of the ranges given by this script.
func createCjkTable() async throws
-> (cjkTable: [UInt32: Bool], cjkDisablingEmojis: [CodePointRange])
{
    var cjkTable: [UInt32: Bool] = [:]
    var cjkDisablingEmojis: [CodePointRange] = []

    // First populate the map with the code points matching the desired East Asian Width values.
    let (cjkRanges, nonCjkRanges) = try await cjkEawRanges()
    cjkRanges.forEachCodePoint {
        cjkTable[$0] = true
    }
    nonCjkRanges.forEachCodePoint {
        cjkTable[$0] = false
    }

    // Remove code points that can be used as emoji
    try await singleCodePointEmojiRanges().forEachCodePoint { codePoint in
        if cjkTable[codePoint] == true {
            cjkDisablingEmojis.append(.single(codePoint))
        }
        cjkTable[codePoint] = false
    }

    // Add in Hangul code points
    try await hangulRanges().forEachCodePoint {
        cjkTable[$0] = true
    }

    // Fill in the ranges that default to an East Asian Width of W
    unassignedAsCjkRanges.forEachCodePoint { codePoint in
        if cjkTable[codePoint] == nil {
            cjkTable[codePoint] = true
        }
    }

    return (cjkTable, cjkDisablingEmojis)
}

extension [UInt32: Bool] {
    /// Processes this map of code points into an equivalent set of code point ranges for those
    /// with a `true` value.
    func codePointRanges() -> [CodePointRange] {
        guard !isEmpty else { return [] }

        var ranges: [CodePointRange] = []
        var rangeStart: UInt32? = nil

        let sortedValues = self.keys.sorted()
        let maxValue = sortedValues.last!

        for codePoint in 0...maxValue {
            if self[codePoint] == true {
                if rangeStart == nil {
                    rangeStart = codePoint
                }
            } else if let start = rangeStart {
                if start == codePoint - 1 {
                    ranges.append(.single(start))
                } else {
                    ranges.append(.range(start...(codePoint - 1)))
                }
                rangeStart = nil
            }
        }

        if let rangeStart {
            if rangeStart == maxValue {
                ranges.append(.single(rangeStart))
            } else {
                ranges.append(.range(rangeStart...maxValue))
            }
        }

        return ranges
    }
}

print("// Generated by make_cjk_ranges.swift")
print("// Ranges reflect Unicode version \(unicodeVersion).")
print("")

var currentExpression = ""
var first = true

for range in try await createCjkTable().cjkTable.codePointRanges() {
    let prefix = first ? "return" : " ||"
    let rangeExpression = "\(prefix) \(range.cExpression)"

    if currentExpression.count + rangeExpression.count > 80 {
        print(currentExpression)
        currentExpression = "   "
    }

    currentExpression += rangeExpression
    first = false
}

print("\(currentExpression);")

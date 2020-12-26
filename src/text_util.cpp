/*
 *  This file is part of mal-gtk.
 *
 *  mal-gtk is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mal-gtk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mal-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "text_util.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <glib.h>
#include <array>

namespace {
    constinit std::array<std::pair<std::string_view, std::u8string_view>, 251> html_entities {{
        { "AElig;", u8"Æ" }, { "Aacute;", u8"Á" }, { "Acirc;", u8"Â" }, { "Agrave;", u8"À" },
        { "Alpha;", u8"Α" }, { "Aring;", u8"Å" }, { "Atilde;", u8"Ã" }, { "Auml;", u8"Ä" },
        { "Beta;", u8"Β" }, { "Ccedil;", u8"Ç" }, { "Chi;", u8"Χ" }, { "Dagger;", u8"‡" },
        { "Delta;", u8"Δ" }, { "ETH;", u8"Ð" }, { "Eacute;", u8"É" }, { "Ecirc;", u8"Ê" },
        { "Egrave;", u8"È" }, { "Epsilon;", u8"Ε" }, { "Eta;", u8"Η" }, { "Euml;", u8"Ë" },
        { "Gamma;", u8"Γ" }, { "Iacute;", u8"Í" }, { "Icirc;", u8"Î" }, { "Igrave;", u8"Ì" },
        { "Iota;", u8"Ι" }, { "Iuml;", u8"Ï" }, { "Kappa;", u8"Κ" }, { "Lambda;", u8"Λ" },
        { "Mu;", u8"Μ" }, { "Ntilde;", u8"Ñ" }, { "Nu;", u8"Ν" }, { "OElig;", u8"Œ" },
        { "Oacute;", u8"Ó" }, { "Ocirc;", u8"Ô" }, { "Ograve;", u8"Ò" }, { "Omega;", u8"Ω" },
        { "Omicron;", u8"Ο" }, { "Oslash;", u8"Ø" }, { "Otilde;", u8"Õ" }, { "Ouml;", u8"Ö" },
        { "Phi;", u8"Φ" }, { "Pi;", u8"Π" }, { "Prime;", u8"″" }, { "Psi;", u8"Ψ" },
        { "Rho;", u8"Ρ" }, { "Scaron;", u8"Š" }, { "Sigma;", u8"Σ" }, { "THORN;", u8"Þ" },
        { "Tau;", u8"Τ" }, { "Theta;", u8"Θ" }, { "Uacute;", u8"Ú" }, { "Ucirc;", u8"Û" },
        { "Ugrave;", u8"Ù" }, { "Upsilon;", u8"Υ" }, { "Uuml;", u8"Ü" }, { "Xi;", u8"Ξ" },
        { "Yacute;", u8"Ý" }, { "Yuml;", u8"Ÿ" }, { "Zeta;", u8"Ζ" }, { "aacute;", u8"á" },
        { "acirc;", u8"â" }, { "acute;", u8"´" }, { "aelig;", u8"æ" }, { "agrave;", u8"à" },
        { "alefsym;", u8"ℵ" }, { "alpha;", u8"α" }, { "and;", u8"∧" },
        { "ang;", u8"∠" }, { "aring;", u8"å" }, { "asymp;", u8"≈" },
        { "atilde;", u8"ã" }, { "auml;", u8"ä" }, { "bdquo;", u8"„" }, { "beta;", u8"β" },
        { "brvbar;", u8"¦" }, { "bull;", u8"•" }, { "cap;", u8"∩" }, { "ccedil;", u8"ç" },
        { "cedil;", u8"¸" }, { "cent;", u8"¢" }, { "chi;", u8"χ" }, { "circ;", u8"ˆ" },
        { "clubs;", u8"♣" }, { "cong;", u8"≅" }, { "copy;", u8"©" }, { "crarr;", u8"↵" },
        { "cup;", u8"∪" }, { "curren;", u8"¤" }, { "dArr;", u8"⇓" }, { "dagger;", u8"†" },
        { "darr;", u8"↓" }, { "deg;", u8"°" }, { "delta;", u8"δ" }, { "diams;", u8"♦" },
        { "divide;", u8"÷" }, { "eacute;", u8"é" }, { "ecirc;", u8"ê" }, { "egrave;", u8"è" },
        { "empty;", u8"∅" }, { "emsp;", u8" " }, { "ensp;", u8" " }, { "epsilon;", u8"ε" },
        { "equiv;", u8"≡" }, { "eta;", u8"η" }, { "eth;", u8"ð" }, { "euml;", u8"ë" },
        { "euro;", u8"€" }, { "exist;", u8"∃" }, { "fnof;", u8"ƒ" }, { "forall;", u8"∀" },
        { "frac12;", u8"½" }, { "frac14;", u8"¼" }, { "frac34;", u8"¾" }, { "frasl;", u8"⁄" },
        { "gamma;", u8"γ" }, { "ge;", u8"≥" }, { "hArr;", u8"⇔" },
        { "harr;", u8"↔" }, { "hearts;", u8"♥" }, { "hellip;", u8"…" }, { "iacute;", u8"í" },
        { "icirc;", u8"î" }, { "iexcl;", u8"¡" }, { "igrave;", u8"ì" }, { "image;", u8"ℑ" },
        { "infin;", u8"∞" }, { "int;", u8"∫" }, { "iota;", u8"ι" }, { "iquest;", u8"¿" },
        { "isin;", u8"∈" }, { "iuml;", u8"ï" }, { "kappa;", u8"κ" }, { "lArr;", u8"⇐" },
        { "lambda;", u8"λ" }, { "lang;", u8"〈" }, { "laquo;", u8"«" }, { "larr;", u8"←" },
        { "lceil;", u8"⌈" }, { "ldquo;", u8"“" }, { "le;", u8"≤" }, { "lfloor;", u8"⌊" },
        { "lowast;", u8"∗" }, { "loz;", u8"◊" }, { "lrm;", u8"\u200E" }, { "lsaquo;", u8"‹" },
        { "lsquo;", u8"‘" }, { "macr;", u8"¯" }, { "mdash;", u8"—" },
        { "micro;", u8"µ" }, { "middot;", u8"·" }, { "minus;", u8"−" }, { "mu;", u8"μ" },
        { "nabla;", u8"∇" }, { "nbsp;", u8" " }, { "ndash;", u8"–" }, { "ne;", u8"≠" },
        { "ni;", u8"∋" }, { "not;", u8"¬" }, { "notin;", u8"∉" }, { "nsub;", u8"⊄" },
        { "ntilde;", u8"ñ" }, { "nu;", u8"ν" }, { "oacute;", u8"ó" }, { "ocirc;", u8"ô" },
        { "oelig;", u8"œ" }, { "ograve;", u8"ò" }, { "oline;", u8"‾" }, { "omega;", u8"ω" },
        { "omicron;", u8"ο" }, { "oplus;", u8"⊕" }, { "or;", u8"∨" }, { "ordf;", u8"ª" },
        { "ordm;", u8"º" }, { "oslash;", u8"ø" }, { "otilde;", u8"õ" }, { "otimes;", u8"⊗" },
        { "ouml;", u8"ö" }, { "para;", u8"¶" }, { "part;", u8"∂" }, { "permil;", u8"‰" },
        { "perp;", u8"⊥" }, { "phi;", u8"φ" }, { "pi;", u8"π" }, { "piv;", u8"ϖ" },
        { "plusmn;", u8"±" }, { "pound;", u8"£" }, { "prime;", u8"′" }, { "prod;", u8"∏" },
        { "prop;", u8"∝" }, { "psi;", u8"ψ" }, { "rArr;", u8"⇒" },
        { "radic;", u8"√" }, { "rang;", u8"〉" }, { "raquo;", u8"»" }, { "rarr;", u8"→" },
        { "rceil;", u8"⌉" }, { "rdquo;", u8"”" }, { "real;", u8"ℜ" }, { "reg;", u8"®" },
        { "rfloor;", u8"⌋" }, { "rho;", u8"ρ" }, { "rlm;", u8"\u200F" }, { "rsaquo;", u8"›" },
        { "rsquo;", u8"’" }, { "sbquo;", u8"‚" }, { "scaron;", u8"š" }, { "sdot;", u8"⋅" },
        { "sect;", u8"§" }, { "shy;", u8"\u00AD" }, { "sigma;", u8"σ" }, { "sigmaf;", u8"ς" },
        { "sim;", u8"∼" }, { "spades;", u8"♠" }, { "sub;", u8"⊂" }, { "sube;", u8"⊆" },
        { "sum;", u8"∑" }, { "sup;", u8"⊃" }, { "sup1;", u8"¹" }, { "sup2;", u8"²" },
        { "sup3;", u8"³" }, { "supe;", u8"⊇" }, { "szlig;", u8"ß" }, { "tau;", u8"τ" },
        { "there4;", u8"∴" }, { "theta;", u8"θ" }, { "thetasym;", u8"ϑ" }, { "thinsp;", u8" " },
        { "thorn;", u8"þ" }, { "tilde;", u8"˜" }, { "times;", u8"×" }, { "trade;", u8"™" },
        { "uArr;", u8"⇑" }, { "uacute;", u8"ú" }, { "uarr;", u8"↑" }, { "ucirc;", u8"û" },
        { "ugrave;", u8"ù" }, { "uml;", u8"¨" }, { "upsih;", u8"ϒ" }, { "upsilon;", u8"υ" },
        { "uuml;", u8"ü" }, { "weierp;", u8"℘" }, { "xi;", u8"ξ" }, { "yacute;", u8"ý" },
        { "yen;", u8"¥" }, { "yuml;", u8"ÿ" }, { "zeta;", u8"ζ" }, { "zwj;", u8"\u200D" },
        { "zwnj;", u8"\u200C" }, { "apos;", u8"'" }, { "quot;", u8"\""}, { "#039;", u8"\u0027"},
        }};

    constinit const std::array<const std::string_view, 3> ignored_entities {"lt;", "gt;", "amp;"};
}

namespace MAL {

    TextUtility::TextUtility()
    {
    }

    namespace {
        char32_t entity_to_char32(const std::string& str, std::string::size_type begin) {
            /* Determine base */
            int base { 10 };
            if (str.at(begin) == 'x') {
                base = 16;
                ++begin;
            }

            /* Convert from string. char32_t is unsigned, careful of narrowing. */
            size_t p { 0 };
            auto s { str.substr(begin) };
            auto l { std::stoul(s, &p, base) };
            if (G_UNLIKELY(s.at(p) != ';')) {
                throw std::invalid_argument("Expected numeric value after &#");
            }

            /* Check range */
            constexpr decltype(l) max { std::numeric_limits<char32_t>::max() };
            if (G_UNLIKELY(l > max)) {
                throw std::out_of_range("HTML entity out of char32_t range");
            }

            /* Return the value */
            return static_cast<char32_t>(l);
        }
    }

    void TextUtility::parse_html_entities(std::string& str)
    {
        auto pos = str.find("&");
        for (; pos != std::string::npos; pos = str.find("&", pos + 1)) {
            /* Determine entity to convert */
            auto end_pos = str.find(";", pos);
            if (G_UNLIKELY(end_pos == std::string::npos))
                continue;
            auto entity = str.substr(pos+1, end_pos - pos);

            /* Ignore valid xml entities */
            if (std::any_of(std::begin(ignored_entities), std::end(ignored_entities),
                            [&](auto &s){ return s == entity; }))
                continue;

            /* Find a UTF-8 representation */
            std::string utf8;

            /* Lookup from known conversions */
            auto iter = std::find_if(std::begin(html_entities), std::end(html_entities), [&entity](const auto &x) {return x.first == entity;});
            if (G_LIKELY(iter != std::end(html_entities))) {
                auto s = iter->second;
                utf8 = std::string(std::begin(s), std::end(s));
                /* std::cout << "Map coverted " << entity << " to " << utf8 << std::endl; */
            } else {
                /* If its not a unicode point, give up. */
                if (G_UNLIKELY(entity.at(0) != '#')) {
                    g_debug("Unhandled HTML entity '%s'. Full text: %s", entity.c_str(), str.c_str());
                    continue;
                }

                /* Convert unicode point to UTF-8 */
                try {
                    char32_t val { entity_to_char32(entity, 1) };
                    utf8 = ucs4conv.to_bytes(val);
                    //html_entities.try_emplace(entity, std::begin(utf8), std::end(utf8));
                    g_debug("Converted %s to %s", entity.c_str(), utf8.c_str());
                } catch (std::exception& e) {
                    g_message("Error converting html integer entity '%s' to UTF-8: %s",
                              entity.c_str(), e.what());
                }
            }

            /* Replace entity with utf8 */
            if (G_LIKELY(utf8.size() > 0)) {
                str.replace(pos, end_pos - pos + 1, utf8);
            }
        }
    }
}

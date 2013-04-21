#include "text_util.hpp"
#include <iostream>
#include <glib.h>

namespace {
	std::map<const std::string, const std::string> initialize_entities() {
		return {
			{ "AElig;", "Æ" }, { "Aacute;", "Á" }, { "Acirc;", "Â" }, { "Agrave;", "À" },
			{ "Alpha;", "Α" }, { "Aring;", "Å" }, { "Atilde;", "Ã" }, { "Auml;", "Ä" },
			{ "Beta;", "Β" }, { "Ccedil;", "Ç" }, { "Chi;", "Χ" }, { "Dagger;", "‡" },
			{ "Delta;", "Δ" }, { "ETH;", "Ð" }, { "Eacute;", "É" }, { "Ecirc;", "Ê" },
			{ "Egrave;", "È" }, { "Epsilon;", "Ε" }, { "Eta;", "Η" }, { "Euml;", "Ë" },
			{ "Gamma;", "Γ" }, { "Iacute;", "Í" }, { "Icirc;", "Î" }, { "Igrave;", "Ì" },
			{ "Iota;", "Ι" }, { "Iuml;", "Ï" }, { "Kappa;", "Κ" }, { "Lambda;", "Λ" },
			{ "Mu;", "Μ" }, { "Ntilde;", "Ñ" }, { "Nu;", "Ν" }, { "OElig;", "Œ" },
			{ "Oacute;", "Ó" }, { "Ocirc;", "Ô" }, { "Ograve;", "Ò" }, { "Omega;", "Ω" },
			{ "Omicron;", "Ο" }, { "Oslash;", "Ø" }, { "Otilde;", "Õ" }, { "Ouml;", "Ö" },
			{ "Phi;", "Φ" }, { "Pi;", "Π" }, { "Prime;", "″" }, { "Psi;", "Ψ" },
			{ "Rho;", "Ρ" }, { "Scaron;", "Š" }, { "Sigma;", "Σ" }, { "THORN;", "Þ" },
			{ "Tau;", "Τ" }, { "Theta;", "Θ" }, { "Uacute;", "Ú" }, { "Ucirc;", "Û" },
			{ "Ugrave;", "Ù" }, { "Upsilon;", "Υ" }, { "Uuml;", "Ü" }, { "Xi;", "Ξ" },
			{ "Yacute;", "Ý" }, { "Yuml;", "Ÿ" }, { "Zeta;", "Ζ" }, { "aacute;", "á" },
			{ "acirc;", "â" }, { "acute;", "´" }, { "aelig;", "æ" }, { "agrave;", "à" },
			{ "alefsym;", "ℵ" }, { "alpha;", "α" }, { "and;", "∧" },
			{ "ang;", "∠" }, { "aring;", "å" }, { "asymp;", "≈" },
			{ "atilde;", "ã" }, { "auml;", "ä" }, { "bdquo;", "„" }, { "beta;", "β" },
			{ "brvbar;", "¦" }, { "bull;", "•" }, { "cap;", "∩" }, { "ccedil;", "ç" },
			{ "cedil;", "¸" }, { "cent;", "¢" }, { "chi;", "χ" }, { "circ;", "ˆ" },
			{ "clubs;", "♣" }, { "cong;", "≅" }, { "copy;", "©" }, { "crarr;", "↵" },
			{ "cup;", "∪" }, { "curren;", "¤" }, { "dArr;", "⇓" }, { "dagger;", "†" },
			{ "darr;", "↓" }, { "deg;", "°" }, { "delta;", "δ" }, { "diams;", "♦" },
			{ "divide;", "÷" }, { "eacute;", "é" }, { "ecirc;", "ê" }, { "egrave;", "è" },
			{ "empty;", "∅" }, { "emsp;", " " }, { "ensp;", " " }, { "epsilon;", "ε" },
			{ "equiv;", "≡" }, { "eta;", "η" }, { "eth;", "ð" }, { "euml;", "ë" },
			{ "euro;", "€" }, { "exist;", "∃" }, { "fnof;", "ƒ" }, { "forall;", "∀" },
			{ "frac12;", "½" }, { "frac14;", "¼" }, { "frac34;", "¾" }, { "frasl;", "⁄" },
			{ "gamma;", "γ" }, { "ge;", "≥" }, { "hArr;", "⇔" },
			{ "harr;", "↔" }, { "hearts;", "♥" }, { "hellip;", "…" }, { "iacute;", "í" },
			{ "icirc;", "î" }, { "iexcl;", "¡" }, { "igrave;", "ì" }, { "image;", "ℑ" },
			{ "infin;", "∞" }, { "int;", "∫" }, { "iota;", "ι" }, { "iquest;", "¿" },
			{ "isin;", "∈" }, { "iuml;", "ï" }, { "kappa;", "κ" }, { "lArr;", "⇐" },
			{ "lambda;", "λ" }, { "lang;", "〈" }, { "laquo;", "«" }, { "larr;", "←" },
			{ "lceil;", "⌈" }, { "ldquo;", "“" }, { "le;", "≤" }, { "lfloor;", "⌊" },
			{ "lowast;", "∗" }, { "loz;", "◊" }, { "lrm;", "\xE2\x80\x8E" }, { "lsaquo;", "‹" },
			{ "lsquo;", "‘" }, { "macr;", "¯" }, { "mdash;", "—" },
			{ "micro;", "µ" }, { "middot;", "·" }, { "minus;", "−" }, { "mu;", "μ" },
			{ "nabla;", "∇" }, { "nbsp;", " " }, { "ndash;", "–" }, { "ne;", "≠" },
			{ "ni;", "∋" }, { "not;", "¬" }, { "notin;", "∉" }, { "nsub;", "⊄" },
			{ "ntilde;", "ñ" }, { "nu;", "ν" }, { "oacute;", "ó" }, { "ocirc;", "ô" },
			{ "oelig;", "œ" }, { "ograve;", "ò" }, { "oline;", "‾" }, { "omega;", "ω" },
			{ "omicron;", "ο" }, { "oplus;", "⊕" }, { "or;", "∨" }, { "ordf;", "ª" },
			{ "ordm;", "º" }, { "oslash;", "ø" }, { "otilde;", "õ" }, { "otimes;", "⊗" },
			{ "ouml;", "ö" }, { "para;", "¶" }, { "part;", "∂" }, { "permil;", "‰" },
			{ "perp;", "⊥" }, { "phi;", "φ" }, { "pi;", "π" }, { "piv;", "ϖ" },
			{ "plusmn;", "±" }, { "pound;", "£" }, { "prime;", "′" }, { "prod;", "∏" },
			{ "prop;", "∝" }, { "psi;", "ψ" }, { "rArr;", "⇒" },
			{ "radic;", "√" }, { "rang;", "〉" }, { "raquo;", "»" }, { "rarr;", "→" },
			{ "rceil;", "⌉" }, { "rdquo;", "”" }, { "real;", "ℜ" }, { "reg;", "®" },
			{ "rfloor;", "⌋" }, { "rho;", "ρ" }, { "rlm;", "\xE2\x80\x8F" }, { "rsaquo;", "›" },
			{ "rsquo;", "’" }, { "sbquo;", "‚" }, { "scaron;", "š" }, { "sdot;", "⋅" },
			{ "sect;", "§" }, { "shy;", "\xC2\xAD" }, { "sigma;", "σ" }, { "sigmaf;", "ς" },
			{ "sim;", "∼" }, { "spades;", "♠" }, { "sub;", "⊂" }, { "sube;", "⊆" },
			{ "sum;", "∑" }, { "sup;", "⊃" }, { "sup1;", "¹" }, { "sup2;", "²" },
			{ "sup3;", "³" }, { "supe;", "⊇" }, { "szlig;", "ß" }, { "tau;", "τ" },
			{ "there4;", "∴" }, { "theta;", "θ" }, { "thetasym;", "ϑ" }, { "thinsp;", " " },
			{ "thorn;", "þ" }, { "tilde;", "˜" }, { "times;", "×" }, { "trade;", "™" },
			{ "uArr;", "⇑" }, { "uacute;", "ú" }, { "uarr;", "↑" }, { "ucirc;", "û" },
			{ "ugrave;", "ù" }, { "uml;", "¨" }, { "upsih;", "ϒ" }, { "upsilon;", "υ" },
			{ "uuml;", "ü" }, { "weierp;", "℘" }, { "xi;", "ξ" }, { "yacute;", "ý" },
            { "yen;", "¥" }, { "yuml;", "ÿ" }, { "zeta;", "ζ" }, { "zwj;", "\xE2\x80\x8D" },
            { "zwnj;", "\xE2\x80\x8C" }, { "apos;", "'" }, { "quot;", "\""}
		};
	}
}

namespace MAL {

    TextUtility::TextUtility() :
        locale("en_US.UTF-8"),
        html_entities(initialize_entities())
    {
    }

    std::string TextUtility::wchar_to_utf8(const std::wstring& wstr) const
    {
        if (std::has_facet<codecvt_t>(locale)) {
            auto& facet = std::use_facet<codecvt_t>(locale);
            std::mbstate_t state = std::mbstate_t();
            std::string out(wstr.size()*3+1, '\0');
            const wchar_t *wstr_last;
            char *str_last;
            auto result = facet.out(state, &*wstr.cbegin(), &*wstr.cend(),
                                    wstr_last, &*std::begin(out), &*std::end(out),
                                    str_last);
            if ( G_LIKELY((result == codecvt_t::ok)) ) {
                return out.substr(0, str_last - &*out.cbegin());
            } else if (result == codecvt_t::partial) {
                std::cerr << "Warning: Partial UCS-4->UTF-8 conversion. "
                          << "Please report to developer" << std::endl;
                return out;
            } else if (result == codecvt_t::error) {
                std::cerr << "Warning: Error during UCS-4->UTF-8 conversion. "
                          << "Please report to developer." << std::endl;
                return std::string();
            } else if (result == codecvt_t::noconv) {
                std::cerr << "Warning: Noconv during UCS-4->UTF-8 conversion? "
                          << "Please report to developer." << std::endl;
                return std::string();
            } else {
                std::cerr << "Warning: Unexpected error during UCS-4->UTF-8 "
                          << " conversion. Please report to developer." 
                          << std::endl;
                return std::string();
            }
        } else {
            std::cerr << "Error: UCS4->UTF8 facet unavailable." << std::endl;
            return std::string();
        }
    }

    void TextUtility::parse_html_entities(std::string& str) const
    {
		auto pos = str.find("&");
		decltype(pos) start = 0;
		while (pos != std::string::npos) {
			auto end_pos = str.find(";", pos);
			if (end_pos == std::string::npos)
				break;
            std::string replacement;
			auto iter = html_entities.find(str.substr(pos+1, end_pos - pos));
            auto this_substring = str.substr(pos+1, end_pos - pos);
			if (iter == std::end(html_entities)) {
                if ( *std::begin(this_substring) == '#' ) {
                    try {
                        wchar_t val;
                        if ( (pos+2) == 'x' ) {
                            val = std::stol(this_substring.substr(2), nullptr, 16);
                        } else {
                            val = std::stol(this_substring.substr(1), nullptr, 10);
                        }
                        replacement = wchar_to_utf8(std::wstring(1, val));

                    } catch (std::exception& e) {
                        std::cerr << "Error converting html integer entity to UTF-8: " 
                                  << e.what() << std::endl;
                    }
                }
            } else {
                replacement = iter->second;
			}
            if (replacement.size() > 0) {
				str.replace(pos, end_pos - pos + 1, replacement);
            }

            start = pos + 1;
			pos = str.find("&", start);
		}
    }
}

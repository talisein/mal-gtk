#include "mal.hpp"
#include "password_dialog.hpp"
#include <curl/curl.h>
namespace {
	extern "C" {
		static size_t curl_write_function(void *buffer, size_t size, size_t nmemb, void *userp) {
			auto const data = std::string(static_cast<char*>(buffer), size*nmemb);
			auto const buf = static_cast<std::string*>(userp);
			buf->append(std::move(data));

			return size*nmemb;
		}

		static void mal_curl_lock_function(CURL* curl, curl_lock_data data, curl_lock_access access, void* userp) {
			auto functor_pair_p = static_cast<MAL::MAL::pair_lock_functor_t*>(userp);
			functor_pair_p->first(curl, data, access);
		}

		static void mal_curl_unlock_function(CURL* curl, curl_lock_data data, void* userp) {
			auto functor_pair_p = static_cast<MAL::MAL::pair_lock_functor_t*>(userp);
			functor_pair_p->second(curl, data);
		}
	}
}

namespace MAL {
	MAL::MAL(std::unique_ptr<UserInfo>&& info) :
		user_info(std::move(info)),
		curl_ebuffer(new char[CURL_ERROR_SIZE]),
		share_lock_functors(new pair_lock_functor_t(std::bind(&MAL::involke_lock_function, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
		                                            std::bind(&MAL::involke_unlock_function, this, std::placeholders::_1, std::placeholders::_2))),
		curl_share(curl_share_init()),
		html_entities(initialize_entities())
	{
		signal_run_password_dialog.connect(sigc::mem_fun(*this, &MAL::run_password_dialog));
		CURLSHcode code;

		code = curl_share_setopt(curl_share.get(), CURLSHOPT_LOCKFUNC, &mal_curl_lock_function);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_UNLOCKFUNC, &mal_curl_unlock_function);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_USERDATA, share_lock_functors.get());
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}

		if (!user_info->has_details()) {
			run_password_dialog();
		}
	}

	void MAL::run_password_dialog() {
		PasswordDialog dialog;
		auto res = dialog.run();
		if (res == 1) {
			if (dialog.get_username().size() > 0 && dialog.get_password().size() > 0) {
				user_info->set_details(dialog.get_username(), dialog.get_password());
			} else {
				std::cerr << "Error: Username and/or password were left blank." << std::endl;
			}
		} else {
			std::cerr << "Error: Password dialog had unexpected exit." << std::endl;
		}
	}

	void MAL::setup_curl_easy(CURL* easy, const std::string& url, std::string* buffer) {
		CURLcode code;
		code = curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, &curl_write_function);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_WRITEDATA, buffer);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_ERRORBUFFER, curl_ebuffer.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_FAILONERROR, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_URL, url.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_SHARE, curl_share.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
	}
	
	std::list<Anime> MAL::get_anime_list_sync() {
		std::list<Anime> out;
		if (!user_info->get_username()) {
			std::cerr << "Error: No username provided." << std::endl;
			return out;
		}

		const std::string url = LIST_BASE_URL + user_info->get_username().get() + "&status=all";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());

		setup_curl_easy(curl.get(), url, buf.get());
		CURLcode code = curl_easy_perform(curl.get());
		
		if (code != CURLE_OK) {
			print_curl_error(code);
			return out;
		}

		parse_entities(*buf);
		out = serializer.deserialize(*buf);

		return out;
	}

	std::list<Anime> MAL::search_anime_sync(const std::string& terms) {
		std::list<Anime> res;
		
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		std::unique_ptr<char, CURLEscapeDeleter> terms_escaped(curl_easy_escape(curl.get(), terms.c_str(), terms.size()));
		const std::string url = SEARCH_BASE_URL + terms_escaped.get();

		setup_curl_easy(curl.get(), url, buf.get());
		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (code != CURLE_OK)
				print_curl_error(code);
			else if (res == 401) {
				signal_run_password_dialog();
			}
		}

		parse_entities(*buf);

		res = serializer.deserialize(*buf);
		
		return res;
	}

	bool MAL::update_anime_sync(const Anime& anime) {
		const std::string url = UPDATED_BASE_URL + std::to_string(anime.series_animedb_id) + ".xml";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		setup_curl_easy(curl.get(), url, buf.get());
		auto xml = serializer.serialize(anime);
		xml.insert(0, "data=");

		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_COPYPOSTFIELDS, xml.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (code != CURLE_OK)
				print_curl_error(code);
			else if (res == 401) {
				signal_run_password_dialog();
			}
		}

		std::cerr << "Response: " << *buf << std::endl;
		if (buf->compare("Updated") == 0) 
			return true;
		else
			return false;
	}

	bool MAL::add_anime_sync(const Anime& anime) {
		const std::string url = ADD_BASE_URL + std::to_string(anime.series_animedb_id) + ".xml";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		setup_curl_easy(curl.get(), url, buf.get());
		auto xml = serializer.serialize(anime);
		xml.insert(0, "data=");
		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_COPYPOSTFIELDS, xml.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (res == 401) {
				signal_run_password_dialog();
			}
		}

		std::cerr << "Response: " << *buf << std::endl;
		if (code == CURLE_OK) {
			signal_anime_added();
			return true;
		} else {
			return false;
		}
	}

	void MAL::print_curl_error(CURLcode code) {
			std::cerr << "Error: " << curl_easy_strerror(code)
			          << ", " << curl_ebuffer.get() << std::endl;
	}

	void MAL::print_curl_share_error(CURLSHcode code) {
			std::cerr << "Error: " << curl_share_strerror(code) << std::endl;
	}

	void MAL::involke_lock_function(CURL*, curl_lock_data data, curl_lock_access) {
		auto iter = map_mutex.find(data);
		if (iter == map_mutex.end()) {
			auto res = map_mutex.insert(std::move(std::make_pair(data, std::move(std::unique_ptr<std::mutex>(new std::mutex())))));
			if (!res.second) {
				std::cerr << "Error: Trying to lock curl data but we can't create the mutex" << std::endl;
				return;
			}
			iter = res.first;
		}

		iter->second->lock();
	}

	void MAL::involke_unlock_function(CURL*, curl_lock_data data) {
		auto iter = map_mutex.find(data);
		if (iter == map_mutex.end()) {
			std::cerr << "Error: Trying to unlock curl data but we don't have the mutex." << std::endl;
			return;
		}

		iter->second->unlock();
	}

	void MAL::parse_entities(std::string& str) const {
		auto pos = str.find("&");
		decltype(pos) start = 0;
		while (pos != std::string::npos) {
			auto end_pos = str.find(";", pos);
			if (end_pos == std::string::npos)
				break;

			auto iter = html_entities.find(str.substr(pos+1, end_pos - pos));
			if (iter == std::end(html_entities)) {
				start = pos + 1;
			} else {
				str.erase(pos, end_pos - pos + 1);
				str.insert(pos, iter->second);
				start = pos + 1;
			}

			pos = str.find("&", start);
		}
	}

	std::map<const std::string, const std::string> MAL::initialize_entities() const {
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
			{ "zwnj;", "\xE2\x80\x8C" }
		};
	}
}

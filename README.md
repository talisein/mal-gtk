mal-gtk
=======
GTK+3 Interface for myanimelist.net


Under Development
=================
myanimelist has an API available again, so I will be creating a new "mal-gtk."
For now this is an inoperable work-in-progress. My current work is to make a
simple CLI tool 'mal-cli' to query the API.

My goals
--------
- C++20
- GTK4 interface
- Flatpak releases
- Local data store in sqlite
- Support user profiles (Switch between accounts, or different tabs for different accounts)

Requirements
------------
C++20 compiler, like gcc 10 or clang 10.
[gtkmm3](https://www.gtkmm.org)
[libsecret](https://wiki.gnome.org/Projects/Libsecret)
[libxml2](http://xmlsoft.org/)
[libcurl](https://curl.haxx.se/libcurl/)
[meson](http://mesonbuild.com/)
[ninja](https://ninja-build.org/)
...and a org.freedesktop.secrets provider such as [gnome-keyring-daemon](https://wiki.gnome.org/Projects/GnomeKeyring).

Building
--------
        # mkdir build
        # cd build
        # meson ..
        # ninja
        # ./src/mal-gtk # run the program
        # sudo ninja install # required.

'ninja install' is required due to needing to install and register the .desktop
file. If you don't want to install to /usr/local (and thus require sudo) just
use meson configure to set the appropriate prefix and the datadir to
~/.local/share

The desktop file defines the x-scheme-handler/net.talinet.malgtk, this is needed
so when you authenticate to myanimelist.net in your browser, the browser can
return the OAuth authentication code back to the app.

### Future Plans

- Set a preferred name (Space Brothers instead of Uchuu Kyoudai)
- Add custom series that aren't in MAL (Akiba Ranger, Game of Thrones)
- User-guided conflict resolution when local cache is different than MAL
- Countdown to next airing
- A .desktop file and icon
- anidb.net backend (Longer term goal)
- Get a .rpm and .deb in Debian/Fedora
- Make builds available for Mac/Windows

### Not Planned

- Updating episode count by monitoring your media player
- Torrent / Tracker integration (Use [flexget](http://www.flexget.com))
- Twitter/Facebook/Twipo integration
- Interface for MAL.net forums/messages
- Realtime chat with people watching the same episode
- Sorting/Renaming files on your hard disk

Competitors
-----------
These are the GUI MAL updaters I know about. There's a ton of CLI-only
clients out there, but that's not the same thing.

- [Taiga](http://taiga.moe/) - Mac/Windows Only, but pretty
  cool. [FOSS](https://github.com/erengy/taiga).
- [Toshocat](https://github.com/tofuness/Toshocat) - Mac/Windows. FOSS.
- [MALU](http://www.malupdater.com/) - Windows only bloatware.
- [Trackma](https://github.com/z411/trackma) - Python, Gtk2. It has a
  cool curses interface, and connects to more than just MAL. FOSS.

IRC --- I'm usually alone in #malgtk on Rizon. I may not respond
immediately, but please feel free to leave a comment.

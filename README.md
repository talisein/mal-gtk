mal-gtk
=======
GTK+3 Interface for myanimelist.net


myanimelist.net's API is [not available](https://myanimelist.net/forum/?topicid=1731860) so this app doesn't currently work
===========================================================================================================================
When the API is available again, it is likely I will need to rework at
least authentication, and possibily the rest of the API as well. I
will leave an update here when I begin that work.

Requirements
------------
[gtkmm3](https://www.gtkmm.org), [libsecret](https://wiki.gnome.org/Projects/Libsecret), [libxml2](http://xmlsoft.org/), [libcurl](https://curl.haxx.se/libcurl/), and a org.freedesktop.secrets
provider such as gnome-keyring-daemon or ksecretservice.

Also [meson](http://mesonbuild.com/) and [ninja](https://ninja-build.org/) to build it.

Building
--------
        # mkdir build
        # cd build
        # meson ..
        # ninja
        # ./src/mal-gtk # run the program
        # sudo ninja install # optional, installs program

mal-gtk is the executable name.

Usage Notes
-----------
- I use mal-gtk everyday, but its not quite "release ready" yet.

- MAL.net has no way to read a Manga's "Scan Group" field.

  While the API does allow writing to this field, at present mal-gtk
  will only save this field locally; I don't want to overwrite any
  value you have on MAL.

- Entry boxes (e.g. Fansub Group or Begin Date) in the Detail View
  will not be saved until you press enter, or you edit a different
  field. Pressing the + button on the episode count will automatically
  save the change, but if you manually modify the episode count you'll
  need to press enter.

- Some 'extended details' don't seem to be saving onto myanimelist.net
  anymore. These are fields like Fansub Group. Changing
  watching/completed status, rating, and changing the viewed episode
  count is working fine. I will try to fix this bug sometime
  soon... (issue #3)

Goals
-----
- Responsive, completely asyncronous design
- Integration with [f.d.o](http://freedesktop.org) Desktop
- Use latest Gtk+ and C++ features

Screenshots
-----------
### Anime List
![Anime List](https://github.com/talisein/mal-gtk/blob/master/images/animelist.jpg?raw=true)

### Manga Search
![Manga Search](https://github.com/talisein/mal-gtk/blob/master/images/mangasearch.jpg?raw=true)

Features
--------
### Implemented
- View Anime and Manga lists
- Search for anime and manga, add them to lists
- Modify fundamental elements:

        Seen Episode/Chapter/Volume
        Score
        Viewing Status (Watching/Dropped/etc)
        Began/Finished dates
        Rewatching flag
- Detailed modification:

        Fansub Group
        Downloaded episode count
        Times rewatched
        Priority
        Rewatch/Reread Value
- Caches a local copy of the anime/manga list
- Modification possible via both the List View and the Detail View

### Current Development Focus
- Pure C GObject implementation for Anime and Manga (90%)
- GListModel / GtkListBox implementation for anime/manga lists (0%)
- UI defined via Glade xml. (0%)

### Planned

- xdg-app release
- Detailed modification:

        Rewatching episode number
        Comments and Tags
        Storage type & value
- Set a preferred name (Space Brothers instead of Uchuu Kyoudai)
- Add custom series that aren't in MAL (Akiba Ranger, Game of Thrones)
- User-guided conflict resolution when local cache is different than MAL
- Countdown to next airing
- A .desktop file and icon
- Use a GtkStack instead of tabs (Gtk+ 3.10)
- anidb.net backend (Longer term goal)
- Get a .rpm and .deb in Debian/Fedora
- Make builds available for Mac/Windows

### Not Planned
Most of these are either bad ideas, better implemented in a specialized
program, or too difficult to get right in 100% of situations:

- Updating episode count by monitoring your media player
- Torrent / Tracker integration (Use [flexget](http://www.flexget.com))
- Twitter/Facebook/Twipo integration
- Interface for MAL.net forums/messages
- Realtime chat with people watching the same episode
- Sorting/Renaming files on your hard disk

These aren't necessarily absolutes; I may end up providing messages
from anidb.net & MAL to telepathy or something someday, but don't look
for these features anytime soon.

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

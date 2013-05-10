mal-gtk
=======
GTK+3 Interface for myanimelist.net

Arch package here: https://aur.archlinux.org/packages/mal-gtk-git/

Requirements
------------
gtkmm3, libsecret, libxml2, libcurl, and a org.freedesktop.secrets
provider such as gnome-keyring-daemon or ksecretservice.

Building
--------
        # autoreconf --force --install
        # ./configure
        # make
        # sudo make install

mal-gtk is the executable name.

Goals
-----
- Responsive, completely asyncronous design
- Integration with [f.d.o](http://freedesktop.org) Desktop
- Use latest Gtk+ and C++ features

Screenshots
-----------
### Anime List

![Anime List](http://github.com/talisein/mal-gtk/blob/master/images/animelist.jpg?raw=true)

### Manga Search
![Manga Search](http://github.com/talisein/mal-gtk/blob/master/images/mangasearch.jpg?raw=true)

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
- Modification possible via both the List View and the Detail View

### Planned
- Detailed modification
        Rewatching episode number
        Fansub Group
        Comments and Tags
        Downloaded episode count
        Times rewatched
        Priority
        Storage type & value

- Cache a local copy of the anime list
- Set a preferred name (Space Brothers instead of Uchuu Kyoudai)
- Add custom series that aren't in MAL (Akiba Ranger, Game of Thrones)
- User-guided conflict resolution when local cache is different than MAL
- Use a GtkStack instead of tabs (Gtk+ 3.10)
- anidb.net backend (Longer term goal)


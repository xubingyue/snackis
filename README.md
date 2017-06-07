# Snackis
#### sugar-free co-creation

### Dependencies
Snackis requires a ```C++1z``` capable compiler/standard library to build, and defaults to using ```clang``` with ```libc++```; it further depends on ```libcurl```, ```libpthread```, ```libsodium``` and ```libuuid``` for core functionality, as well as ```NCurses``` for the UI. All dependencies except ```libcurl``` and ```NCurses``` are statically linked into the provided [Linux/64-binaries](https://github.com/andreas-gone-wild/snackis/tree/master/dist).

### Getting started
If you're running Linux/64, grab the ```snackis```-executable from ```/dist``` and put it in its own separate catalog somewhere. Snackis is designed to run completely self-contained and will create anything it needs in the directory where it's started.

#### Master Password
Each snackis installation is protected by a master password that is used to encrypt the database; pick a good one, because changing it means re-encrypting the entire database.

![login example](images/login.png?raw=true)

#### Settings
Once inside, the first thing you probably want to do is to have a look at the settings. Type ```settings``` and press ```Enter``` to open the settings form. Imap and Smtp settings are needed to do anything beyond encrypting/decrypting information for personal use.

![settings example](images/settings.png?raw=true)

### License
Snackis is licensed under the GNU General Public License Version 3.

### Help
[This is your chance](https://www.paypal.me/c4life) to help Snackis move forward faster; $700/month allows me to work full-time on the project, every contribution counts. Thank you!
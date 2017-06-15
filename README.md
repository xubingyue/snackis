# Snackis
#### sugar-free co-creation

### Introduction
[Snackis](http://groups.csail.mit.edu/mac/classes/6.805/articles/crypto/cypherpunks/may-crypto-manifesto.html) is aiming for something that might be described as a post-modern enigma-device; a simple and convenient one-stop shop for most secure communication needs. It provides a curses-based UI on top of [libsodium](https://github.com/jedisct1/libsodium) and supports using any regular email-account as transport. Peers, encryption keys, history and settings are stored locally, encrypted using a master password. At present, Snackis supports key-exchange via invites/accepts and encryption/decryption of arbitrary data for a specific peer and group-discussions/feeds.

### Status
Snackis UI is currently being ported from NCurses to Gtk+ since it proved to cumbersome to implement the needed functionality. The new version is nearing feature parity, only the final pieces for posting are missing; and is provided as ```dist/gcurses```. Once the conversion is complete, the previous UI will be deprecated.

### Dependencies
Snackis requires a ```C++1z```-capable compiler and standard library to build, and defaults to using [clang](http://releases.llvm.org/download.html#4.0.0) with ```libc++```; it further depends on ```libcurl```, ```libpthread```, ```libsodium``` and ```libuuid``` for core functionality, as well as ```NCurses``` and ```GTK+ 3```for the UI.

```
tar -xzf clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
cd clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04
sudo cp -R * /usr/local/

sudo apt-get install libcurl4-openssl-dev libsodium-dev libuuid1 libncurses-dev libgtk-3-dev
```

```
git clone https://github.com/andreas-gone-wild/snackis.git
mkdir snackis/build
cd snackis/build
cmake ..
make
```

### Getting started
If you're running ```Linux/64```, grab the ```snackis```-executable from [here](https://github.com/andreas-gone-wild/snackis/tree/master/dist), otherwise you'll have to build it yourself. Put the executable in its own directory. Snackis is designed to run completely self-contained and will create everything it needs in the directory where it's started.

#### Navigating and Editing
Pressing ```Return``` moves focus to next field. Fields with names ending in ```=``` provide a fixed set of alternatives, use ```Tab``` to cycle or type in field to search. ```Home```, ```End```, ```Backspace``` and arrow keys work as expected. Pressing ```Delete``` deletes from current position until end of field.

#### Master Password
Each snackis installation is protected by a master password that is used to encrypt the database; pick a good one, because changing it means re-encrypting the entire database.

![login example](images/login.png?raw=true)

#### Settings
Once inside, the first thing you probably want to do is to have a look at the settings. Type ```settings``` and press ```Enter``` to open the settings form. Imap/Smtp-settings are needed to do anything beyond encrypting/decrypting data for personal use.

![settings example](images/settings.png?raw=true)

#### Encryption
Snackis supports encrypting arbitrary data for decryption by a specific peer. Type ```encrypt``` and press ```Return``` in the reader to open the encryption form. Files are loaded from the load-folder, and saved to the save-folder specified in ```Settings```. On top of being encrypted, the resulting file is cryptographically signed and the signature is verified when decrypting. Encoding converts the output to hexadecimal text to enable transferring encrypted data reliably over text protocols like email, it also rougly doubles the size of the output.

![encrypt example](images/encrypt.png?raw=true)

#### Decryption
To decrypt files that have been encrypted by Snackis, put them in the load-folder and type ```decrypt``` and press return in the reader to open the decryption form. Decoding is mandatory for files that were encoded as part of the encryption process.

![decrypt example](images/decrypt.png?raw=true)

#### Sending Invites
Type ```invite``` in the reader and press ```Return``` to open the invite form. Snackis will send an email containing your public key to the specified address. The easiest way to see how it works is to try with your own address.

![invite example](images/invite.png?raw=true)

Don't forget to send the actual message. Snackis stores all outgoing messages in an outbox that may be emptied at any time by typing ```send``` and pressing ```Return``` in the reader.

![send example](images/send.png?raw=true)

#### Messages
All Snackis-messages look more or less the same to the outside world. Invites are the exception by only being encoded, not encrypted; since the sender by definition doesn't know the receivers encryption-key yet.

![message example](images/message.png?raw=true)

#### The Inbox
All incoming traffic arrives in the inbox, type ```fetch``` and press ```Return``` to fetch messages and ```inbox``` to open the inbox form.

![inbox example](images/inbox.png?raw=true)

#### Peers
Whenever an invite is accepted/rejected, a message is sent to inform the inviting peer. Peers are uniquely identified by their email address and may be invited any number of times, encryption-keys are exchanged each time an invitation is accepted.

![accept example](images/accept.png?raw=true)

#### Threads
Snackis provides support for threaded group-communication, type 'post' and press return in the reader to open the post form.

![post example](images/post.png?raw=true)

### Algorithms
Snackis delegates anything concerning encryption to [libsodium](https://github.com/jedisct1/libsodium). The IETF-variant of ```ChaCha20-Poly1305``` is used to encrypt the master password and database, while ```XSalsa20``` with ```X25519``` key-exchange and ```Poly2305 MAC```-authentication is used for everything else. The master password is hashed using ```Scrypt```, and the password is then encrypted using the hash as key for future validation.

### License
Snackis is licensed under the GNU General Public License Version 3.

### Help
[This is your chance](https://www.paypal.me/c4life) to help Snackis move forward faster, every contribution counts. Thank you!
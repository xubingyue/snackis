# Snackis
#### sugar-free co-creation

### Dependencies
Snackis requires a ```C++1z``` capable compiler/standard library to build, and defaults to using ```clang``` with ```libc++```; it further depends on ```libcurl```, ```libpthread```, ```libsodium``` and ```libuuid``` for core functionality, as well as ```NCurses``` for the UI. All dependencies except ```libcurl``` and ```NCurses``` are statically linked into the provided [Linux/64-binaries](https://github.com/andreas-gone-wild/snackis/tree/master/dist).

### Getting started
If you're running Linux/64, grab the ```snackis```-executable from [here](https://github.com/andreas-gone-wild/snackis/tree/master/dist) and put it in its own directory. Snackis is designed to run completely self-contained and will create everything it needs in the directory where it's started.

#### Master Password
Each snackis installation is protected by a master password that is used to encrypt the database; pick a good one, because changing it means re-encrypting the entire database.

![login example](images/login.png?raw=true)

#### Settings
Once inside, the first thing you probably want to do is to have a look at the settings. Type ```settings``` and press ```Enter``` to open the settings form. Imap and Smtp settings are needed to do anything beyond encrypting/decrypting information for personal use.

![settings example](images/settings.png?raw=true)

#### Encryption
Snackis supports encrypting arbitrary data for decryption by a specific peer. Type ```encrypt``` and press ```Return``` in the reader to open the encryption form. Files are loaded from the load-folder specified in ```Settings```. On top of being encrypted, the resulting file is cryptographically signed and the signature is verified when decrypting. Encoding converts the output to hexadecimal text to enable transferring the encrypted data reliably over text protocols like email, it also rougly doubles the size of the output.

![encrypt example](images/encrypt.png?raw=true)

#### Decryption
To decrypt files that have been encrypted by Snackis, put them in the load-folder and type ```decrypt``` and press return in the reader to open the decryption form. Files that were encrypted with encoding turned on need decoding turned on to be decrypted.

![decrypt example](images/decrypt.png?raw=true)

#### Sending Invites
Type ```invite``` in the reader and press ```Return``` to open the invite form. Snackis will sent an email containing your public key to the specified address. Try it with your own address to get an idea of how it works.

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

### Algorithms
Snackis delegates anything concerning encryption to [libsodium](https://github.com/jedisct1/libsodium). The IETF-variant of ```ChaCha20-Poly1305``` is used to encrypt the master password and database, while ```XSalsa20``` with ```X25519``` key exchange and ```Poly2305 MAC``` authentication is used for everything else. The master password is hashed using ```Scrypt```, and the password is then encrypted using the hash as key for future validation.

### License
Snackis is licensed under the GNU General Public License Version 3.

### Help
[This is your chance](https://www.paypal.me/c4life) to help Snackis move forward faster; $700/month allows me to work full-time on the project, every contribution counts. Thank you!
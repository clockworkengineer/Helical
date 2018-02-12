#**Helical: A Simple Qt Based SSH Client**

Helical is a Qt based SSH client for connecting to SSH servers and allowing the execution of remote commands , running of a remote shell or accessing the remote filesystem using SFTP. It supports password and public key user authorization and public key server verification though ".ssh/known_hosts".

The client is written mostly in Qt/C++ but it uses a set of Antik wrapper classes for talking to 'C' library [libssh](https://www.libssh.org/)  which provides the client side SSH support.

A remote shell session takes place inside a window that emulates a vt100 terminal (supports a subset of vt100 escape sequences) that is currently on fixed size 80x24. Output from any remotely executed commands (stdout/stderr) is sent to the session log.

The SFTP file browser component is yet to be unimplemented.

#**To Do List**

- SFTP client
- Custom view/model implementaiton for vt100 to support character attributes/color.
- Support for more than running session.
### 8-1

> When we execute the following code, we find that it displays the same number twice, even though the two users have different IDs in the password file. Why is this? `printf("%s %s\n", getpwuid(uid1)->pw_name, getpwuid(uid2)->pw_name);` [Errata](https://www.man7.org/tlpi/errata/index.html)

- As can be seen in its manual `The return value may point to a static area, and may be overwritten by subsequent calls to getpwent(3), getpwnam(), or getpwuid().`
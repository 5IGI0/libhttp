# LIBHTTP

**Warning** :

This library has not been tested on windows.

If it does not work on windows, I encourage you to pull request.

## TODO

- [X] http requests
	- [X] IPv4
	- [ ] IPv6
	- [ ] auto-resolve
	- [X] Connection timeout
	- [ ] Read-timeout
- [X] requests maker
	- [X] unstandardised method
	- [X] custom headers
	- [ ] request with body
- [X] response parser
	- [X] HTTP status
		- [X] HTTP version
		- [X] status code
		- [ ] status text
	- [X] headers
		- [X] parse
		- [ ] Custom writer function
	- [X] Body
		- [X] parse
		- [ ] Custom writer function
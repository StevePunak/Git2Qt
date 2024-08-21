# libgit2qt

## Description
This library provides C++/Qt language bindings to libgit2. 
In large part, it is a port of the [libgit2sharp](https://github.com/libgit2/libgit2sharp) which provided a nice OO object model to work from.

## Building

> [!TIP] 
> This project requires Qt 6.

### Just The Library

Get the code: 

    `git clone https://github.com/StevePunak/libgit2qt.git` 

1. Start Qt Creator
2. Open the CMakeLists.txt in the root folder. 
3. Select a kit to build.
4. Build the library

### The Library and a Program With Examples
There is a submodules project which includes the library and a program with some examples. 
This is probably the best way to get started for most people as there is currently no documentation (working on it).

1. Get the code:

    `git clone https://github.com/StevePunak/Git2QtMains.git` 
    
2. Get the test repository:

    `git clone https://github.com/StevePunak/GitTesting.git` 

3. Start Qt Creator
4. Open the CMakeLists.txt in the root folder. 
5. Select a kit to build.
6. Build the project
7. Run the example program with the following parameters    

    `auto --local-path /path/to/test-repository/GitTesting`
    
In the debugger, set the command line arguments to point to the test repository you cloned in step 2 as shown.
    
![To run in the debugger](/../assets/assets/ss-run-config.png)
    


## Usage
Here are some simple sample usages:
### Simple clone

```
Repository* repo = Commands::clone("https://github.com/StevePunak/GitTesting.git", "/home/beavis/src/GitTesting");
```
Supplying credentials and getting progress are performed via callback and are demonstrated in the example code.

### Find a branch

```
Branch branch = _repository->findLocalBranch("master");
if(branch.isNull() == false) {
    ...
}
```

### Stage and commit all modified files

```
StatusEntry::List modified = _repository->status().modified();
if(_repository->stage(modified) == true) {
    Signature signature("beavis", "beavis@butthead.com");
    Commit commit = _repository->commit("This is my great commmit message", signature, signature);
    if(commit.isNull()) {
        // handle commit failure
    }
}
```


## Support
The main way to ask questions and submit feature requests for now is through [discussions](https://github.com/StevePunak/libgit2qt/discussions). 

File bug reports [in the normal manner](https://github.com/StevePunak/libgit2qt/issues).

## Roadmap
I'm going to continue to iterate through my normal git use and add features as I need them. The plan is to fully implement all the features available in libgit2sharp.

## Contributing
Of course, contributions are much appreciated!

As of this writing, the code is very fresh and there are plenty of things to do:
* Better manipulation of remotes
* Windows support for declspecs
* Better submodule support
* CI / CD for nightly builds
* Implement test units (the sample code is intended to be converted to test units)
* Doxygen

Pick something, create a branch and submit a pull request!

## Authors and acknowledgment
The inspiration for this project mainly came from [libgit2sharp](https://github.com/libgit2/libgit2sharp).

I wanted to create a nice object-oriented git library and a great base for my object model was waiting right there.

Thanks to libgit2 and libgit2sharp for the great foundation to work from.

## License
This code is licensed under the Apache license. Free as in free beer.

## Project status
Project is in early development but is already useful and being used in my own applications.

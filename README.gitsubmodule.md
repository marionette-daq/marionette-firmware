
# Git Submodules for this project

## What are the submodules used in this project?

* Submodules are annotated in the .gitmodules file at the top level of the repository

```
> cat .gitmodules 
[submodule "ChibiOS-RT"]
	path = ChibiOS-RT
	url = https://github.com/ChibiOS/ChibiOS-RT.git
>

```

## Initializing and using

* To initialize after cloning:

```
git submodule update --init
```

* To update:

```
git submodule update
```

* For further infomation please reference: 

```
man git submodule
```



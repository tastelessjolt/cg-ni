# cg-ni
A rudimentary modelling framework made using OpenGL  

## Link to report
[here](file://./index.html)

## Controls

### Mode Switch

`M` Modelling Mode  
`I` Inspection Mode  

`Esc` or `Q` Quit Program  

### Modelling Mode

`Left Mouse Click` - Add a point  
`Shift + Left Mouse Click` - Remove the last added point  
`C` Close the polygon started  

`Ctrl + Z` Undo create the last triangle
`Ctrl + Y` Redo create the last 'undo-ed' triangle

`U` Set the color in `R G B` format in float in the terminal  

`Z` Move drawing plane out  
`X` Move drawing plane in  

`W` Camera up  
`A` Camera left  
`S` Camera down  
`D` Camera right  

*Rotation not available in modelling mode*  

### Inspection Mode  

`R` Move camera so that centrod of the model is at the centre of the screen  

`Z` Camera out  
`X` Camera in  
`W` Camera up  
`A` Camera left  
`S` Camera down  
`D` Camera right  

`Left` Rotate left about Y  
`Right` Rotate right about Y  
`Up` Rotate in about X  
`Down` Rotate out about X  
`PgUp` Rotate anti-clock about Z  
`PgDown` Rotate clock about Z  

### Saving/Loading Operations

`K` - Save the current model
	File name input via terminal (without extension)

`L` - Load a model
	File name input via terminal (without extension)

#### TO DO

- ~~File Open/Close Error Handling~~
- ~~Terminal Feedback/Prompts~~
	- ~~State Information~~
		- ~~Mode~~
		- ~~Position of drawing place~~
		- ~~Current drawing color~~
	- ~~Prompts~~
		- ~~File name prompts~~
		- ~~File error feedbacks~~

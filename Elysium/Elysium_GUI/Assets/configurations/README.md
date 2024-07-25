# Configurations
Each subdirectory represents a configuration of valves and sensors, with the corresponding P&ID. The name of the subdirectory indicates the configuration name, the file names within are required to be fixed so that the GUI can access them. I.e., there must files titled `sensors.cfg`, `valves.cfg`, and `P&ID.png`.

## Format of configuration files

Each row (or possibly pair of rows) represents one element that should be added to the GUI. Every row is a set of comma seperated values (no additional spaces) representing a set of arguments.

The first arguments identify the object and provide the parameters required for its initialization. The final arguments, which may go on the second row&mdash;this has not been implemented yet&mdash;will indicate additional placement info to the GUI. For example, should the sensor be independent or grouped with the Pressure Sensors or where on the P&ID should it be located.

The order matters, as the elements are added to the GUI from the top down, starting at the beginning of the file.

### For `sensors.cfg` the arguments are given in the following order:
`ID,name,unit`$\newline$
`group,type`

The first line is self explanatory; `group` indicates that the sensor should be placed in the GUI in a seperated area for all sensors in that group, titled with `group`. The ordering is still first is top, and the same applies to the groups (first group top left, then top right, the second row left, second row right, etc.). If it is left blank (i.e., the line line starts with a `,`), then it is grouped but without a title. The `type` indicates if the sensor is a directly measured quanitity (`measured`), or derived from the others (`derived`). Additional `derived` quantities will require modifying GUI_DAQ_Window(.h/.cpp) to handle their calculations.

### For `valves.cfg` the arguments are given in the following order:
`class,ID,name,*args`

The `class` has three values: `Valve`, `Solenoid`, and `LA_Ball`, indicating the `Valve`, `Solenoid_Valve`, and `LA_Ball_Valve` classes respectively.

The `*args` depends on the class. For `Valve` it indicates the `state` parameter, for `Solenoid` it indicates the `normally_open` parameter, and for `LA_Ball` it will be two values indicating `state` then `is_open`.

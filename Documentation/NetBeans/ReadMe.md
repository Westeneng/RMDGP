# You can use NetBeans to devellop RMDGP
> This guide is based on NetBeans IDE 12.3 on Ubuntu 20.10
>
> - Install standard Apache NetBeans IDE with standard plugins
> - Install the C/C++ plugin. Menu Tools/Plugins tab "available Plugins".
>                   It was in the Nebeans 8.2 Plugin Portal, so you have to enable that first in the tab "Settings"
>
> Put the license template file in the appropriate location  
>  On my laptop it was at: ~/snap/netbeans/common/data/12.2/config/Templates/Licenses/license-gpl20_RMDGP.txt
>
## Create a project in the following way
> - ... todo
>  
> - if you don't use cmake for the unit tests, you have to add the cppunit library manually. 
>> 1. In project browser select with the right mouse button "Test Files" and choose properties.
>> 1. Then click Linker categorie. 
>> 1. Click Libraries .... 
>> 1. Choose 'Add Option' 
>> 1. Choose 'Other Option' and fill in "-lcppunit"

<?php
    Class MenuItem {
        var $ID,
        $StoreID,
        $MenuID,
        $Name,
        $Description,
        $Category,
        $PricingType,
        $Cost1,
        $Cost2,
        $Cost3,
        $Cost4,
        $Cost5,
        $Type1,
        $Type2,
        $Type3,
        $Type4,
        $Type5;

        function MenuItem (){
        }

        function setMenuItemVar ($varname, $varvalue)
        {
            $this->$varname = $varvalue;
        }

        function getMenuItem($ID)
        {
            //function retrieves information from the database about the item.
        }
        
    }
?>
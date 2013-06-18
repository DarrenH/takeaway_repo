<?php
    Class Store {
        var $ID,
        $OwnerID,
        $Name,
        $Type,
        $Website,
        $DeliveryAreas,
        $CuisineType,
        $CuisineTypeName,
        $MenuIDs,
        $MenuID,
        $DLMenus,
        $MenuItemList,
        $FormattedAddress,
        $Rating,
        $Flag,
        $URLName,
        $ReviewUserID,
        $ReviewText,
        $DeliveryType,
        $DeliveryInformation,
        $FreeDeliveryAmount,
        $MaxRadius,
        $DeliveryCost,
        $DeliveryHTML,
        $OfferIDs,
        $FeaturedImages,
        $LogoAbsoluteURL,
        $OverallRating,
        $TypeText,
        $Locations,
        $SetLocationID,
        $IMEI,
        $APIPass,
        $Online,
        $OpeningHours,
        $OHCopy,
        $OHStatus;

        function Store() {
        }

        function setStoreVar($varname,$varvalue)
        {
            $this->$varname = $varvalue;
        }

        function setStoreVars($varnames,$varvalues) {
            $count = 0;
            foreach($varnames as $varname)
            {
                $this->$varname = $varvalues[$count];
                $count++;
            }
        }

        function getStore($sID = null,$test=null)
        {
            //retrieves this information from database
            if (isset($sID))
            {
                $this->setStoreVar('ID', $sID);
            }

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'type', 1 => 'owner_id', 2 => 'name', 3 => 'cuisine_type', 4 => 'halal', 5 => 'logo', 6 => 'website', 7 => 'urlname');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $this->ID);

            $rs = $DBA->selectQuery(DBSTORETABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setStoreVar('Type', $res['type']);
                    $this->setStoreVar('OwnerID', $res['owner_id']);
                    $this->setStoreVar('Name', $res['name']);
                    $this->setStoreVar('CuisineType', $res['cuisine_type']);
                    $this->setStoreVar('CuisineTypeName', $this->getCuisineTypeName());
                    $this->setStoreVar('Halal', $res['halal']);
                    $this->setStoreVar('Website', $res['website']);
                    $this->setStoreVar('URLName', $res['urlname']);
                    $this->setStoreVar('Locations', $this->getLocations());
                    $this->formatAllAddresses();
                    $this->formatAllAddresses('print');
                    $this->formatAllAddresses('short');
                    $this->formatAllAddressesSchema();
                    $this->getOpeningHours();
                    //$this->getTypeText();
                    //$this->getAPIInfo();
                    if (isset($test))
                    {
                        print_r($this);
                        print_r($res);
                    }
                }
            }
        }

        function createStore()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'max(id) as id');
            $rs = $DBA->selectQuery(DBSTORETABLE, $fields);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $newid = $res['id'];
                }
            }
            unset($DBA, $rs, $res, $fields);
            if ($newid == '')
            {
              $newid = 0;
            }
            $newid += 1;
            $this->setStoreVar('ID', $newid);
            unset($newid);

            //creates new this record in database and saves info
            $this->geolocate($this->PCode, $this->House, $this->Street, $this->Town, $this->County);
            $fields = array(0 => 'id', 1 => 'owner_id', 2 => 'name', 3 => 'type', 4 => 'house', 5 => 'street', 6 => 'town', 7 => 'county', 8 => 'pcode', 9 => 'contactphone', 10 => 'contactemail', 11 => 'cuisine_type', 12 => 'geolat', 13 =>'geolong');
            $fieldvals = array(0 => $this->ID, 1 => $this->OwnerID, 2 => addslashes($this->Name), 3 => $this->Type, 4 => $this->House, 5 => $this->Street, 6 => $this->Town, 7 => $this->County, 8 => $this->PCode, 9 => $this->ContactPhone, 10 => $this->ContactEmail, 11 => $this->CuisineType, 12 => $this->GeoLat, 13 => $this->GeoLong);
            $DBA = new DatabaseInterface();
            $rs = $DBA->insertQuery(DBSTORETABLE, $fields, $fieldvals);
            unset($DBA, $rs, $fields, $fieldvals);
        }

        function updateStore()
        {
            //updates this information

        }

        function deleteStore()
        {
            //removes all info relating to the this

        }

        function getLocations()
        {
            $location = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'location_id', 1 => 'house', 2 => 'address_line_2', 3 => 'street', 4 => 'address_line_4', 5 => 'town', 6 => 'county', 7 => 'pcode', 8 => 'contactphone', 9 => 'secondaryphone', 10 => 'contactemail',  11 => 'geolat', 12 => 'geolong', 13 => 'delivers', 14 => 'delivery_type', 15 => 'id as dbid', 16 => 'online', 17 => 'mindeliveryamount', 18 => 'imei', 19 => 'ppadd', 20 => 'pdq', 21 => 'pdqphone', 22 => 'pdqmobile', 23 => 'mastercard', 24 => 'maestro', 25 => 'visael', 26 => 'visa', 27 => 'solo', 28 => 'switch', 29 => 'orderemail');
            $idfields = array(0 => 'store_id');
            $idvals = array(0 => $this->ID);

            $rs = $DBA->selectQuery(DBSTORELOCATIONTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if (!isset($res['geolat']))
                    {
                        $coords = geolocate($res['pcode'], $res['house'], $res['address_line_2'], $res['street'], $res['address_line_4'], $res['town'], $res['county']);
                        $res['posstr'] = $coords['lat'] . ', ' . $coords['long'];
                    }
                    else
                    {
                        $res['posstr'] = $res['geolat'] . ', ' . $res['geolong'];
                    }
                    array_push($location, $res);
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);
            return $location;
        }

        function formatAddress($type = null)
        {
            $fa = $this->House . ' ';
            if ($this->Add2 != '')
            {
              $fa .= $this->Add2 . ' ';
            }
            $fa .= $this->Street;
            if ($type=='print')
            {
                $fa .= ',\n';
            }
            else
            {
                $fa .= ',<br />';
            }
            if ($this->Add4 != '')
            {
              $fa .= $this->Add4;
              if($type=='print')
              {
                  $fa .= ',\n';
              }
              else
              {
                  $fa .= ',<br />';
              }
            }
            $fa .= $this->Town;
            if($type=='print')
            {
                $fa .= ',\n';
            }
            else
            {
                $fa .= ',<br />';
            }
            $fa .= $this->County . ' ' . $this->PCode;
            if ($type=='print')
            {
                $this->setStoreVar('FormattedPrintAddress', $fa);
            }
            else
            {
                $this->setStoreVar('FormattedAddress', $fa);
            }
        }

        function formatAllAddressesSchema() {
            $temploc = array();
            foreach($this->Locations as $local)
            {
                $streetaddress = $local['house'] . ' ';
                if ($local['address_line_2'] != '')
                {
                    $streetaddress .= $local['address_line_2'] . ' ';
                }
                $streetaddress .= $local['street'];

                $fa = '<div class="phonenum">';
                $fa .= '<p class="lead">' . $local['contactphone'] .'</p>';
                $fa .= '</div>';
                $fa .= '<div class="address">';
                $fa .= '<address itemprop="address" itemscope itemtype="http://scheam.org/PostalAddress">';
                $fa .= '<span itemprop="streetAddress">' . $streetaddress . '</span>, ';
                if ($local['address_line_4'] != '') {
                    $fa.= '<span>' . $local['address_line_4'] . '</span>, ';
                }
                $fa .= '<span itemprop="addressLocality">' . $local['town'] . '</span>,<br /> ';
                $fa .= '<span itemprop="addressRegion">' . $local['county'] . '</span> ';
                $fa .= '<span itemprop="postalCode">' . $local['pcode'] . '</span>';
                $fa .= '</address>';
                $fa .= '</div>';

                $local['schema_address'] = $fa;
                 array_push($temploc, $local);
            }
            $this->setStoreVar('Locations', $temploc);
        }

        function formatAllAddresses($type = null)
        {
            $temploc = array();
            foreach($this->Locations as $local)
            {
                $fa = $local['house'] . ' ';
                if ($local['address_line_2'] != '')
                {
                  $fa .= $local['address_line_2'] . ' ';
                }
                $fa .= $local['street'];
                if ($type=='print')
                {
                    $fa .= ',\n';
                }
                else
                {
                    $fa .= ',<br />';
                }
                if ($local['address_line_4'] != '')
                {
                  $fa .= $local['address_line_4'];
                  if($type=='print')
                  {
                      $fa .= ',\n';
                  }
                  else
                  {
                      $fa .= ',<br />';
                  }
                }
                if ($type!='short')
                {
                    $fa .= $local['town'];
                    if($type=='print')
                    {
                        $fa .= ',\n';
                    }
                    else
                    {
                        $fa .= ',<br />';
                    }
                }
                if ($type!='short')
                {
                    $fa .= $local['county'] . ' ' . $local['pcode'];
                }
                else
                {
                    $fa .= $local['pcode'];
                }
                if ($type=='print')
                {
                    //$this->setStoreVar('FormattedPrintAddress', $fa);
                    $local['formattedprintaddress'] = $fa;
                }
                elseif ($type=='short')
                {
                    $local['shortaddress'] = $fa;
                }
                else
                {
                    //$this->setStoreVar('FormattedAddress', $fa);
                    $local['formattedaddress'] = $fa;
                }
                array_push($temploc, $local);
            }
            $this->setStoreVar('Locations', $temploc);
        }

        function setCuisineType($ctype)
        {
            $this->setStoreVal('CuisineType', $ctype);
        }

        function getCuisineType()
        {
            return $this->CuisineType;
        }

        function getCuisineTypeName()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $this->CuisineType);
            $rs = $DBA->selectQuery(DBCUISINETYPESTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $name = $res['name'];
                }
            }

            unset($rs, $res, $fields, $idfields, $idvals, $DBA);

            return ($name);
        }

        function geoLocate($pcode, $house = null, $street = null, $town = null, $county = null, $retset = 0, $add2 = null, $add4 = null)
        {
             $key = GAPI_KEY;
             if ((isset($house)) && (!is_null($house)))
             {
                //house number set
                $url = $house;
             }

             if ((isset($street)) && (!is_null($street)))
             {
                //street set
                if ($url != '')
                {
                    $url .= '+';
                }
                $url .= $street;
             }

             if ((isset($add2)) && (!is_null($add2)))
             {
                 //address line 2 set
                 if ($url != '')
                 {
                     $url .= '+';
                 }
                 $url .= $add2;
             }

             if ((isset($town)) && (!is_null($town)))
             {
                //town set
                if ($url != '')
                {
                    $url .= '+';
                }
                $url .= $town;
             }

             if ((isset($add4)) && (!is_null($add4)))
             {
                 //address line 4 set
                 if ($url != '')
                 {
                     $url .= '+';
                 }
                 $url .= $add4;
             }
             
             if ((isset($county)) && (!is_null($county)))
             {
                //county set
                if ($url != '')
                {
                    $url .= '+';
                }
                $url .= $county;
             }

             if ((isset($pcode)) && (!is_null($pcode)))
             {
                //pcode set
                if ($url != '')
                {
                    $url .= '+';
                }
                $url .= $pcode;
             }
             // Loop through each row, submit HTTP request, output coordinates
             $mapaddress = urlencode($url);

             // Desired address
             $url = "http://maps.google.com/maps/geo?q=$mapaddress&output=xml&key=$key";

             // Retrieve the URL contents
             $page = file_get_contents($url);

             // Parse the returned XML file
             $xml = new SimpleXMLElement($page);

             // Parse the coordinate string
             list($longitude, $latitude, $altitude) = explode(",", $xml->Response->Placemark->Point->coordinates);
             if ($retset == 0)
             {
                 $this->setStoreVar('GeoLong', $longitude);
                 $this->setStoreVar('GeoLat', $latitude);
             }
             else
             {
                $retarray = array('lat' => $latitude, 'long' => $longitude);
                return $retarray;
             }
        }

        function getDLMenus()
        {
            $menus = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'smallimg', 1 => 'largeimg', 2 => 'id');
            $idfields = array(0 => 'supplier_id',1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

            $rs = $DBA->selectQuery(DBDLMENUSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && (!$rs[1]==0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($menus, $res);
                }
            }
            $this->setStoreVar('DLMenus', $menus);
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);
            return true;
        }

        function getOLMenu()
        {
            $menuids = array();
            $DBA = new DatabaseInterface();

            $fields = array(0 => 'distinct(id) as id');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

            $rs = $DBA->selectQuery(DBOLMENUTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($menuids, $res);
                }
            }
            $this->setStoreVar('MenuIDs', $menuids);
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);

            if (count($menuids) > 0)
            {
                foreach ($menuids as $id)
                {
                    $Menu = new Menu();
                    $Menu->setMenuVar('ID', $id['id']);
                    $Menu->setMenuVar('StoreID', $this->ID);
                    $Menu->setMenuVar('LocationID', $this->SetLocationID);
                    $Menu->getMenu();

                    if (((count($menuids) > 1) && ($Menu->Status == 1)) || (count($menuids) == 1))
                    {
                        $this->setStoreVar('MenuID', $Menu->ID);
                        $this->setStoreVar('MenuCatList', $Menu->Categories);
                        //$this->setStoreVar('MenuItemList', $Menu->getMenuItems());
                        $this->setStoreVar('MenuItemList', $Menu->getMenuItems3());
                        break;
                    }
                }

                if ($this->MenuItemList == "")
                {
                    $this->setStoreVar('MenuItemList', 'None');
                }
            }
            else
            {
                $this->setStoreVar('MenuItemList', 'None');
            }
        }

        function getDeliveryDetails2()
        {
            //type 1 - 2 choices with 3 options each creating 9 scenarios
            //type 2 - postcode delivery
            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
            $rs = $DBA->selectQuery(DBSTOREDELIVERYTYPETABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                //there is a record in the database for this locations delivery
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $type = $res['type'];
                    switch ($type) {
                        case 1:
                            //standard delivery model
                            $choice = array();
                            $choice = explode('/', $res['choices']);
                            $max_radius = $res['max_radius'];
                            $min_order_value = $res['min_order_value'];
                            break;
                        case 2:
                            unset($max_radius);
                            $min_order_value = $res['min_order_value'];
                            break;
                    }
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            //now we get the info table records and push them into an array
            $info = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'var1', 1 => 'var2', 2 => 'var3');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
            $order = 'id';
            $rs = $DBA->selectQuery(DBSTOREDELIVERYINFOTABLE, $fields, $idfields, $idvals, $order);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($info, $res);
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            $temp = array('type' => $type, 'choices' => $choice, 'max_radius' => $max_radius, 'min_order_value' => $min_order_value, 'variables' => $info);
            $this->setStoreVar('DeliveryInformation', $temp);


            switch ($Store->DeliveryInformation['type']) {
                case 1:
                    switch ($Store->MaxRadius)
                    {
                        case 1:
                            $overlayimg = ONE_MILE_RADIUS;
                            $mapzoom = 14;
                            break;
                        case 2:
                            $overlayimg = TWO_MILE_RADIUS;
                            $mapzoom = 13;
                            break;
                        case 3:
                            $overlayimg = THREE_MILE_RADIUS;
                            $mapzoom = 13;
                            break;
                        case 4:
                            $overlayimg = FOUR_MILE_RADIUS;
                            $mapzoom = 12;
                            break;
                        case 5:
                            $overlayimg = FIVE_MILE_RADIUS;
                            $mapzoom = 12;
                            break;
                    }
                    break;
                case 2:
                    unset($overlayimg);
                    break;
            }

            if (!isset($mapzoom))
            {
                $mapzoom = 16;
            }

            $this->setStoreVar("MapZoomLVL",$mapzoom);
            $this->SetStoreVar("MapOverlay", $overlayimg);
        }

        function getMenuCategoryNavigation()
        {
            $restr = '';
            if (count($this->MenuCatList) >= 1)
            {
                //run through and create menu bar...
                //just li's
                foreach ($this->MenuCatList as $category)
                {
                    if ($category['itemcount'] >=1)
                    {
                        $retstr .= '<li><a href="'. $category['navlink'] . '" title="' . $category['name'] . '">' . $category['name'] . '&nbsp;<small>(' . $category['itemcount'] . ')</small></a></li>';
                    }
                }
            }
            else
            {
                $retstr = '<li>No Categories</li>';
            }
            return($retstr);
        }

        /*function getDeliveryDetails()
        {
            //delivery has 3 different choices
            //1 - Delivery free over £x
            //2 - Delivery £x per mile
            //3 - Mulitple delivery areas by postcode
            $areas = array();
            $DBA = new DatabaseInterface();

            switch ($this->Locations[$this->SetLocationID]['delivery_type']) {
                case 1:
                    $fields = array(0 => 'free_delivery_amount', 1=> 'max_radius', 2 => 'cost');
                    $idfields = array(0 => 'store_id', 1 => 'location_id');
                    $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

                    $rs = $DBA->selectQuery(DBSTOREDELIVERYAREASTABLE, $fields, $idfields, $idvals);
                    if (!isset($rs[2]))
                    {
                        while ($res = mysql_fetch_array($rs[0]))
                        {
                             array_push($areas, $res);
                        }
                    }
                    unset($rs, $res, $fields, $idfields, $idvals, $orderby, $DBA);
                    break;
                case 2:
                    $fields = array(0 => 'radius', 1 => 'cost');
                    $idfields = array(0 => 'store_id', 1 => 'location_id');
                    $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
                    $order_by = 'radius';

                    $rs = $DBA->selectQuery(DBSTOREDELIVERYAREASTABLE, $fields, $idfields, $idvals, $orderby);

                    if (!isset($rs[2]))
                    {
                        while ($res = mysql_fetch_array($rs[0]))
                        {
                            array_push($areas, $res);
                        }
                    }
                    unset($rs, $es, $fields, $idfields, $idvals, $orderby, $DBA);
                    break;
                case 3:
                    $fields = array(0 => 'pcode', 1 => 'radius', 2 => 'cost');
                    $idfields = array(0 => 'store_id', 1 => 'location_id');
                    $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

                    $rs = $DBA->selectQuery(DBSTOREDELIVERYAREASTABLE, $fields, $idfields, $idvals);

                    if (!isset($rs[2]))
                    {
                        while ($res = mysql_fetch_array($rs[0]))
                        {
                            array_push($areas, $res);
                        }
                    }
                    unset($rs, $es, $fields, $idfields, $idvals, $orderby, $DBA);
                    break;
            }
            $this->setStoreVar('DeliveryAreas', $areas);
        } */

        function prepareDeliveryDetails2($scp = false)
        {
            if ($scp == true)
            {
                $clienttxt = "the customer";
                $singular = "their";
            }
            else
            {
                $clienttxt = "you";
                $singular = "your";
            }
            switch($this->DeliveryInformation['type']) {
                case 1:
                    //standard delivery
                    $choicetot = $this->DeliveryInformation['choices'][0] . $this->DeliveryInformation['choices'][1];
                    switch ($choicetot) {
                        case 11:
                            $str = '<p>All orders are charged at &pound;' . $this->DeliveryInformation['variables'][0][1] . '.</p>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                        case 12:
                            $str = '<p>All orders are charged according to the distance ' . $clienttxt .' are from this store</p>';
                            $str .= '<p>Please check the table below for details.</p>';
                            //table of distances and costs
                            $str .= '<table class="del-areas table table-bordered table-condensed table-striped">';
                            $str .= '<tbody>';
                            $str .= '<tr><th>Distance(Miles)</th><th>Cost(&pound;)</th></tr>';
                            for ($i = 0; $i <= 4; $i++)
                            {
                                //each of these is a value check if its set and show if it is
                                if (isset($this->DeliveryInformation['variables'][$i]['var1']))
                                {
                                    $str .= '<tr><td>Up to ' . $this->DeliveryInformation['variables'][$i]['var1'] . ' miles</td><td>' . ($this->DeliveryInformation['variables'][$i]['var2'] == 0 ? "FREE" : number_format($this->DeliveryInformation['variables'][$i]['var2'], 2)) . '</td></tr>';
                                }
                            }
                            $str .= '</tbody>';
                            $str .= '</table>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $str .= '<p>Minimum Order Value for Delivery: ' . $this->DeliveryInformation['min_order_value'] . '</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                        case 13:
                            $str = '<p>This store does not deliver.</p>';
                            break;
                        case 21:
                            $str = '<p>This store gives free delivery on all orders over &pound;' . $this->DeliveryInformation['variables'][0][0] . '.</p>';
                            $str .= '<p>All other orders are charged at &pound;' . $this->DeliveryInformation['variables'][0][1] . '</p>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                        case 22:
                            $str = '<p>This store gives free delivery on all orders over &pound;' . $this->DeliveryInformation['variables'][0][0] . '.</p>';
                            $str .= '<p>All other orders are charged according to the distance ' . $clienttxt . ' are from this store.</p>';
                            $str .= '<p>Please check the table below for details.</p>';
                            //table of distances and costs
                            $str .= '<table class="del-areas table table-bordered table-condensed table-striped">';
                            $str .= '<tbody>';
                            $str .= '<tr><th>Distance(Miles)</th><th>Cost(&pound;)</th></tr>';
                            for ($i = 0; $i <= 4; $i++)
                            {
                                //each of these is a value check if its set and show if it is
                                if (isset($this->DeliveryInformation['variables'][$i]['var2']))
                                {
                                    $str .= '<tr><td>' . $this->DeliveryInformation['variables'][$i]['var2'] . '</td><td>' . ($this->DeliveryInformation['variables'][$i]['var3'] == 0 ? "FREE" : number_format($this->DeliveryInformation['variables'][$i]['var3'], 2)) . '</td></tr>';
                                }
                            }
                            $str .= '</tbody>';
                            $str .= '</table>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                        case 23:
                            $str = '<p>This store gives free delivery on all orders over &pound;' . $this->DeliveryInformation['min_order_value'] . '.</p>';
                            $str .= '<p>If ' . $singular . ' order does not qualify for free delivery ' . $clienttxt . ' will have to collect ' . $singular . ' order.</p>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                        case 31:
                            $str = '<p>This store gives free delivery on all orders which are requested up to ' . $this->DeliveryInformation['variables'][0][0] . ' miles radius of ' . $storetxt . '</p>';
                            $str .= '<p>All other orders are charged at &pound;' . $this->DeliveryInformation['variables'][0][1] . '</p>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                        case 32:
                            $str = '<p>This store gives free delivery on all orders which are requested up to ' . $this->DeliveryInformation['variables'][0][0] . ' miles radius of the store.</p>';
                            $str .= '<p>All other orders are charged according to the distance ' . $clienttxt . ' are from this store.</p>';
                            $str .= '<p>Please check the table below for details.</p>';
                            //table of distances and costs
                            $str .= '<table class="del-areas table table-bordered table-condensed table-striped">';
                            $str .= '<tbody>';
                            $str .= '<tr><th>Distance(Miles)</th><th>Cost(&pound;)</th></tr>';
                            for ($i = 0; $i <= 4; $i++)
                            {
                                //each of these is a value check if its set and show if it is
                                if (isset($this->DeliveryInformation['variables'][$i]['var2']))
                                {
                                    $str .= '<tr><td>' . $this->DeliveryInformation['variables'][$i]['var2'] . '</td><td>' . ($this->DeliveryInformation['variables'][$i]['var3'] == 0 ? "FREE" : number_format($this->DeliveryInformation['variables'][$i]['var3'], 2)) . '</td></tr>';
                                }
                            }
                            $str .= '</tbody>';
                            $str .= '</table>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                        case 33:
                            $str = '<p>This store gives free delivery on all orders which are requested up to ' . $this->DeliveryInformation['variables'][0][0] . ' miles radius of ' . $storetxt . '.</p>';
                            $str .= '<p>If ' . $singular . ' order does not qualify for free delivery ' . $clienttxt . 'will have to collect ' . $singular . ' order.</p>';
                            $max_radius = $this->DeliveryInformation['max_radius'];
                            $str .= '<p>Max delivery radius: ' . $max_radius . ' Miles</p>';
                            $this->setStoreVar('MaxRadius', $max_radius);
                            break;
                    }
                    break;
                case 2:
                    //postcode delivery
                    $str = '<p>This store only delivers to certain postcodes. If ' . $clienttxt . ' are not inside one of these then this store will not deliver.</p>';
                    if (($this->DeliveryInformation != 0) && ($this->DeliveryInformation != ""))
                    {
                        $str .= '<p>Orders must be over &pound;' . number_format($this->DeliveryInformation['min_order_value'],2) . ' to qualify for delivery.</p>';
                    }
                    $str .= '<p>Please check the table below for details.</p>';
                    $str .= '<table class="table table-bordered table-condensed table-striped">';
                    $str .= '<tbody>';
                    $str .= '<tr><th>Postcode</th><th>Cost(&pound;)</th></tr>';
                    for ($i = 0; $i <= 9; $i++)
                    {
                        if (isset($this->DeliveryInformation['variables'][$i]['var1']))
                        {
                            //each of these is a value check if its set and show if it is
                            $str .= '<tr><td>' . $this->DeliveryInformation['variables'][$i]['var1'] . '</td><td>' . ($this->DeliveryInformation['variables'][$i]['var2'] == 0 ? "FREE" : number_format($this->DeliveryInformation['variables'][$i]['var2'],2)) . '</td></tr>';
                        }
                    }
                    $str .= '</tbody>';
                    $str .= '</table>';
                    break;
            }
            $this->setStoreVar('DeliveryHTML', $str);
        }

        /*function prepareDeliveryDetails($scp=null)
        {
            switch ($this->Locations[$this->SetLocationID]['delivery_type']) {
                case 1:
                    $this->setStoreVar('FreeDeliveryAmount', $this->DeliveryAreas[0]['free_delivery_amount']);
                    $this->setStoreVar('MaxRadius', $this->DeliveryAreas[0]['max_radius']);
                    $this->setStoreVar('DeliveryCost', $this->DeliveryAreas[0]['cost']);
                    if (is_null($scp))
                    {
                        $str = '<p>Please check the table below for delivery details</p>';
                    }
//                    $str = '<p>Delivery from ' . $this->Name . ' is FREE on all all orders over £' . $this->FreeDeliveryAmount . ' and costs £' . $this->DeliveryCost . ' otherwise.</p>';
//                    $str .= '<p>Delivery is within a ' . $this->MaxRadius . ' mile radius';
                    $str .= '<table class="del-areas">';
                    $str .= '<tbody>';
                    $str .= '<tr><th>Order Amount</th><th>Cost</th></tr>';
                    $str .= '<tr><td>£1 - £' . $this->DeliveryAreas[0]['free_delivery_amount'] . '</td><td>' . $this->DeliveryAreas[0]['cost'] . '</td></tr>';
                    $str .= '<tr><td>£' . ($this->DeliveryAreas[0]['free_delivery_amount'] + 0.01) . ' and above</td><td>FREE</td></tr>';
                    $str .= '<tr><td>Up to a distance of ' . $this->DeliveryAreas[0]['max_radius'] . ' miles</td><td>&nbsp;</td></tr>';
                    $str .= '</tbody>';
                    $str .= '</table>';
                    $this->setStoreVar('DeliveryHTML', $str);
                    break;
                case 2:
                    if (is_null($scp))
                    {
                        $str = '<p>Please check the table below for delivery details.</p>';
                    }
                    $str .= '<table class="del-areas">';
                    $str .= '<tbody>';
                    $str .= '<tr><th>Radius (Miles)</th><th>Cost(£)</th></tr>';
                    foreach($this->DeliveryAreas as $area)
                    {
                        $str .= '<tr><td>' . $area['radius'] . '</td><td>';
                        if ($area['cost'] != '0.00')
                        {
                            $str .= $area['cost'];
                        }
                        else
                        {
                            $str .= 'FREE';
                        }
                        $str .= '</td></tr>';
                    }
                    $str .= '</tbody>';
                    $str .= '</table>';
                    $str .= '<p>If you are <a href="' . REGISTER_LINK . '" title="Register for a free account">registered on the site</a> and have filled in your address delivery will be added automatically onto your cart, otherwise the system will add the delivery charge when you specify your delivery address.</p>';
                    $this->setStoreVar('DeliveryHTML', $str);
                    break;
                case 3:
                    if (is_null($scp))
                    {
                        $str = '<p>Please check the table below for delivery details</p>';
                    }
                    $str .= '<table class="del-areas">';
                    $str .= '<tbody>';
                    $str .= '<tr><th>Postcode</th><th>Radius (Miles)</th><th>Cost</th></tr>';
                    foreach ($this->DeliveryAreas as $area)
                    {
                        $str .= '<tr><td>' . $area['pcode'] . '</td><td>' . $area['radius'] . '</td><td>' . $area['cost'] . '</td></tr>';
                    }
                    $str .= '</tbody>';
                    $str .= '</table>';
                    $this->setStoreVar('DeliveryHTML', $str);
                    break;
            }
        } */

        function getReviews()
        {
            $reviews = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'content', 1 => 'user_id', 2 => 'id', 3 => 'review_date');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

            $rs = $DBA->selectQuery(DBREVIEWSTABLE, $fields, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    $reviewUser = new Customer();
                    $reviewUser->setUserVar('ID', $res['user_id']);
                    $reviewUser->getUser();
                    $reviewUser->getAvatar();

                    $content = html_entity_decode($res['content'], ENT_QUOTES, 'ISO-8859-1');
                    $date = date("D dS M Y", $res['review_date']);
                    $isodate = date("Y-m-d");
                    $review = array('id' => $res['id'], 'username' => $reviewUser->Username, 'user_id' => $reviewUser->ID, 'user_avatar' => $reviewUser->AvatarImageString, 'content' => $content, 'review-date' => $date, 'iso-date' => $isodate);
                    array_push($reviews, $review);
                    unset($reviewUser);
                }
            }
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);

            $this->setStoreVar('Reviews', $reviews);
        }

        function getFlag()
        {
           $this->setStoreVar('Flag', get_flag($this->CuisineTypeName));
        }

        function getRating()
        {
            $avgrating = get_rating($this->ID);
            if ($avgrating == '')
            {
              $avgrating = 0;
            }
            $this->setStoreVar('Rating', $avgrating);
        }

        function getOffers($locationid = null)
        {
            $ids = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            if (!isset($locationid))
            {
                $locationid = $this->SetLocationID;
            }
            $idvals = array(0 => $this->ID, 1 => $locationid);

            $rs = $DBA->selectQuery(DBSPECIALOFFERSTABLE, $fields, $idfields, $idvals);
            if((!isset($rs[2])) && ($rs[1] != 0))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    array_push($ids, $res['id']);
                }
            }
            else
            {
                unset($ids);
            }

            unset($DBA, $res, $rs, $fields, $idfields, $idvals);

            $this->setStoreVar('OfferIDs', $ids);
        }

        function getOfferbyID($offerid)
        {
            $return = array();

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description', 2 => 'expires', 3 => 'tandc', 4 => 'how_to_redeem');
            $idfields = array(0 => 'store_id', 1 => 'id');
            $idvals = array(0 => $this->ID, 1 => $offerid);

            $rs = $DBA->selectQuery(DBSPECIALOFFERSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($return ,$res);
                }
            }

            unset($rs, $res, $fields, $idfields, $idvals, $DBA);

            return($return);
        }

        function getOfferCloseDate($id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'expires');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $id);

            $rs = $DBA->selectQuery(DBSPECIALOFFERSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $date = $res['expires'];
                }
            }

            unset($rs, $res, $DBA, $fields, $idfields, $idvals);
            return($date);
        }

        function showOffer($type = 'tab', $dbid, $link = false, $logged = null, $scp = null, $edit = null)
        {
            if ($type == 'tab')
            {
                $cssclass = 'offer-tab';
            }
            else
            {
                $cssclass = 'offer-page';
            }

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description', 2 => 'expires', 3 => 'associated_ids', 4 => 'offer_type', 5 => 'associated_image', 6 => 'expires', 7 => 'location_id');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $dbid);
            $order = 'expires desc';
            $rs = $DBA->selectQuery(DBSPECIALOFFERSTABLE, $fields, $idfields, $idvals, $order);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if ($res['expires'] < time())
                    {
                        //expired
                        $expired = true;
                    }
                    else
                    {
                        $expired = false;
                    }
                    unset($str);
                    /*if ($type == 'page')
                    {
                        $str = '<div class="offer-top"></div>';
                    }*/
                    $str .= '<div class="' . $cssclass . '">';
                    $str .= '<span class="supplier-logo">';
                    if ($link == true)
                    {
                        $str .= '<a href="' . SITE_PATH . STORE_PATH . $this->URLName . '/';
                        if ($res['location_id'] == 0)
                        {
                        }
                        else
                        {
                            $str .= $res['location_id'] . '/';
                        }
                        $str .= '" title="' . $this->Name . '">';
                    }
                    if ((isset($res['associated_image'])) && (!is_null($res['associated_image'])) && ($res['associated_image'] != ''))
                    {
                        $str .= '<img src="' . SITE_PATH . 'supplier-image-folder/' . $this->URLName . '/offers/small/' . $res['associated_image'] . '" alt="' . $res['name'] . '" />';
                        $str .= '<span class="offer-img-del"><a href="' . SITE_PATH . 'delete-offer-image/' . $this->ID . '/' . $this->SetLocationID . '/' . $dbid . '/" title="delete this image"><span>Delete this image</span></a></span>';
                    }
                    else
                    {
                        $str .= $this->Logo;
                    }
                    if ($link == true)
                    {
                        $str .= '</a>';
                    }
                    $str .= '</span>';
                    if ($expired == true)
                    {
                        $str .= '<span class="exp-sm"></span>';
                    }
                    $str .= '<span class="supplier-info">';
                    $str .= '<h3>' . $res['name'] . '</h3>';
                    if ($type == 'page')
                    {
                        $str .= '<p class="at">at</p>';
                        $str .= '<h4><a href="' . SITE_PATH . STORE_PATH . $this->URLName . '/';
                        if ($res['location_id'] == 0)
                        {
                        }
                        else
                        {
                            $str .= $res['location_id'] . '/';
                        }
                        $str .= '" title="' . $this->Name . '">' . $this->Name . '</a></h4>';
                    }
                    $str .= '<span class="offer-text">' . $res['description'] . '</span>';
                    $str .= '<span class="expires">Offer Expire' . ($expired == true ? 'd' : 's') . ': ' . date("D jS M Y", $res['expires']) . '</span>';
                    if (((isset($logged)) && ($expired != true)) && (!isset($scp)))
                    {
                        $str .= '<span class="download"><a href="' . SITE_PATH . 'download_offer/' . $dbid . '/" target="blank" title="' . $res['name'] . ' - downloadable voucher"><img src="' . SITE_PATH . 'images/download-offer.png" alt="' . $res['name'] . ' - downloadable voucher" /></a></span>';
                    }
                    elseif (isset($scp))
                    {
                        $str .= '<span class="download">';
                        $str .= '<a href="' .SUPPLIER_ACCOUNT_LINK . $this->ID . '/' . $this->SetLocationID . '/offer-details/' . $dbid . '/" title="Edit this offer">Edit this offer</a>';
                        $str .= '</span>';
                        $str .= '<span class="download">';
                        $str .= '<a href="' . SITE_PATH . 'delete-offer/' . $this->ID . '/' . $this->SetLocationID . '/' . $dbid . '/" title="Delete this offer">Delete this offer</a>';
                        $str .= '</span>';
                        $str .= '<span class="confirm">';
                        $str .= '<p>Delete this offer?</p>';
                        $str .= '<form name="conf-del" action="' . SITE_PATH . 'delete-offer/" method="post">';
                        $str .= '<fieldset><legend>Are you sure?</legend>';
                        $str .= '<input type="hidden" name="conf" value="1" />';
                        $str .= '<input type="hidden" name="storeid" value="' . $this->ID . '" />';
                        $str .= '<input type="hidden" name="locationid" value="' . $this->SetLocationID . '" />';
                        $str .= '<input type="hidden" name="offerid" value="' . $dbid . '" />';
                        $str .= '<input type="submit" name="yes" value="Yes" /><input type="submit" name="no" value="no" />';
                        $str .= '</fieldset>';
                        $str .= '</form>';
                        $str .= '</span>';
                    }
                    elseif ($expired == true)
                    {
                        $str .= '';
                    }
                    else
                    {
                        $str .= '<span class="download"><a href="' . REGISTER_LINK . '" title="Register for a free account">Register for a free account to dowload this offer.</a></span>';
                    }
                    $str .= '</span>';
                    if (isset($scp))
                    {
                        if ((isset($res['associated_image'])) && (!is_null($res['associated_image'])) && ($res['associated_image'] != ''))
                        {}
                        else
                        {
                            $str .= '<div class="offer-img-upload">';
                            $str .= '<form name="offer-' . $dbid . '" action="' . SITE_PATH . 'upload-offer-image/" method="post" enctype="multipart/form-data">';
                            $str .= '<input type="hidden" name="MAX_FILE_SIZE" value="200000" />';
                            $str .= '<input type="hidden" name="storeid" value="' . $this->ID . '" />';
                            $str .= '<input type="hidden" name="locationid" value="' . $this->SetLocationID . '" />';
                            $str .= '<input type="hidden" name="offerid" value="' . $dbid . '" />';
                            $str .= '<label for="offerimage">Upload an image file for this offer.</label>';
                            $str .= '<input type="file" name="offerimage" />';
                            $str .= '<p class="smallprint">Images must be of types .JPG, .PNG or .GIF. Images must also be 459px398px or smaller and under 200KB</p>';
                            $str .= '<input type="submit" class="sdetssub" name="sdetssub" value="Save" rel="imupload" />';
                            $str .= '</form>';
                            $str .= '</div>';
                        }
                    }
                    $str .= '</div>';
                    /*if ($type == 'page')
                    {
                        $str .= '<div class="offer-bottom"></div>';
                    }*/
                }
            }
            return($str);
        }

        function editOffer($dbid)
        {

            return($str);
        }

        function getLogo($size)
        {
            $absurl = SERVER_ROOT . 'supplier-image-folder/' . $this->URLName . '/' . $size . '/logo.jpg';
            $url = SITE_PATH . 'supplier-image-folder/' . $this->URLName . '/' . $size . '/logo.jpg';

            $this->setStoreVar('LogoAbsoluteURL', $absurl);
            if (file_exists($absurl))
            {
                $this->setStoreVar('Logo', '<img src="' . $url . '" alt="' . $Store->Name . '" />');
            }
            else
            {
                $this->setStoreVar('Logo', '<img src="' . SITE_PATH . 'supplier-image-folder/default/' . $size . '/logo.jpg" alt="' . $Store->Name . '" />');
            }
        }

        function saveReview()
        {
            $id = getmaxid(DBREVIEWSTABLE);
            $time = time();
            $fields = array(0 => 'id', 1 => 'user_id', 2 => 'store_id', 3 => 'location_id', 4 => 'content', 5 => 'review_date');
            $fieldvals = array(0 => $id, 1 => $this->ReviewUserID, 2 => $this->ID, 3 => $this->SetLocationID, 4 => $this->ReviewText, 5 => $time);

            $DBA = new DatabaseInterface();
            $rs = $DBA->insertQuery(DBREVIEWSTABLE, $fields, $fieldvals);
            return ($id);
        }

        function getFeaturedImages()
        {
            $images = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'filename', 1 => 'imagetype');
            $idfields = array(0 => 'store_id',1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
            $order = 'imagetype asc';

            $rs = $DBA->selectQuery(DBFEATUREDIMAGESTABLE, $fields, $idfields, $idvals, $order);

            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($images, $res);
                }
            }

            unset($fields, $idfields, $idvals, $DBA, $res, $rs);

            $this->setStoreVar('FeaturedImages', $images);
        }

        function getOverallRating()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'avg(rating) as averagerating', 1 => 'count(rating) as totalr');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

            $rs = $DBA->selectQuery(DBRATINGSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if ($res['averagerating'] != '')
                    {
                        $this->Locations[$this->SetLocationID]['rating_count'] = $res['totalr']/3;
                        $this->Locations[$this->SetLocationID]['overall_rating'] = floor($res['averagerating']);
                        /*$this->setStoreVar('OverallRating', floor($res['averagerating']));
                        $this->setStoreVar('RatingCount', $res['totalr']/3);*/
                    }
                    else
                    {
                        $this->Locations[$this->SetLocationID]['rating_count'] = 0;
                        $this->Locations[$this->SetLocationID]['overall_rating'] = 0;
/*                        $this->setStoreVar('OverallRating', '0');
                        $this->setStoreVar('RatingCount', '0');*/
                    }
                }
            }
            else
            {
                $this->Locations[$this->SetLocationID]['rating_count'] = 0;
                $this->Locations[$this->SetLocationID]['overall_rating'] = 0;
                /*$this->setStoreVar('OverallRating', 0);
                $this->setStoreVar('RatingCount', '0');*/
            }
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);
        }

        function getTypeText()
        {
            if ($this->Type == 'T')
            {
                $this->setStoreVar('TypeText', 'Takeaway');
            }
            elseif ($this->Type == 'R')
            {
                $this->setStoreVar('TypeText', 'Restaurant');
            }
            else
            {
                $this->setStoreVar('TypeText', 'Restaurant & Takeaway');
            }
        }

        function getAPIInfo()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'imei', 1 => 'api_password');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
            $rs = $DBA->selectQuery(DBSTORELOCATIONTABLE, $fields, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setStoreVar('IMEI', $res['imei']);
                    $this->setStoreVar('APIPass', $res['api_password']);
                }
            }
        }

        function APICall($url, $apistr, $actionstr, $ping=null, $fget=null)
        {
            if (isset($fget))
            {
                $page = curl_get_contents($url . $apistr . $actionstr);
            }
            else
            {
                $page = file_get_contents($url . $apistr . $actionstr);
            }
            $Xml = new SimpleXMLElement($page);

            if ($Xml->status != 'success')
            {
                $error = $Xml->Error;
            }
            else
            {
                if (isset($ping))
                {
                    return $Xml->printer_status;
                }
                else
                {
                    return 'success';
                }
            }
        }

        function sendMessage($message,$fget1=null)
        {
            $url = "https://sms2printer.com/manage/api/";
            $apistr = "?apiver=1&imei=" . $this->IMEI . "&password=" . $this->APIPass;
            $actionstr = "&action=ping";

            $result = $this->APICall($url, $apistr, $actionstr, 1, $fget1);

            if ($result == "online")
            {
                $actionstr = "&action=print&print_message=". $message;
                $result2 = $this->APICall($url, $apistr, $actionstr);

                if ($result2 == 'success')
                {
                    return ('success');
                }
            }
            else
            {
                if (($result == '') || (!isset($result)))
                {
                    return('error');
                }
                else
                {
                    return ('offline');
                }
            }
        }

        function pingStore()
        {
            $url = "https://sms2printer.com/manage/api/";
            $apistr = "?apiver=1&imei=" . $this->IMEI . "&password=" . $this->APIPass;
            $actionstr = "&action=ping";

            $result = $this->APICall($url, $apistr, $actionstr, 1);

            if ($result[0] == 'online')
            {
                $temp = array('online' => 'Y');
                $this->Locations[$this->SetLocationID] = array_merge($this->Locations[$this->SetLocationID], $temp);
                return ('success');
            }
            else
            {
                $temp = array('online' => 'N');
                $this->Locations[$this->SetLocationID] = array_merge($this->Locations[$this->SetLocationID], $temp);
                return (false);
            }
        }

        function UpdateOnlineStatus()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'online');
            $fieldvals = array(0 => $this->Locations[$this->SetLocationID]['online']);
            $idfield = 'id';
            $idval = $this->Locations[$this->SetLocationID]['dbid'];

            $rs = $DBA->updateQuery(DBSTORELOCATIONTABLE, $fields, $fieldvals, $idfield, $idval);

            unset($DBA, $fields, $fieldvals, $idfield, $idval);
        }

        function getOpeningHours()
        {
            $temp = array();
            foreach($this->Locations as $local)
            {
                $DBA = new DatabaseInterface();
                $fields = array(0 => '*');
                $idfields = array(0 => 'store_id', 1 => 'location_id');
                $idvals = array(0 => $this->ID, 1 => $local['location_id']);

                $rs = $DBA->selectQuery(DBSTOREOPENINGHOURSTABLE, $fields, $idfields, $idvals);
                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        array_push($temp, $res);
                    }
                }

                unset($rs, $res, $fields, $idfields, $idvals, $DBA);
            }
            $this->setStoreVar('OpeningHours', $temp);
        }

        function prepareOpeningHours()
        {
            $open = "closed";
            //show all opening hours in table.
            $dayname = date('D');
            $ampm = date('a');
            $hour = date('G');

            unset($ohstr);
            for ($i ==0; $i <= 6; $i++)
            {
                switch ($i) {
                    case 0:
                        //monday
                        $day = 'Mon';
                        $vals = array('mon_day','mon_night');
                        break;
                    case 1:
                        //tuesday
                        $day = 'Tue';
                        $vals = array('tues_day','tues_night');
                        break;
                    case 2:
                        //wednesday
                        $day = 'Wed';
                        $vals = array('weds_day','weds_night');
                        break;
                    case 3:
                        //thursday
                        $day = 'Thu';
                        $vals = array('thurs_day','thurs_night');
                        break;
                    case 4:
                        //friday
                        $day = 'Fri';
                        $vals = array('fri_day','fri_night');
                        break;
                    case 5:
                        //saturday
                        $day = 'Sat';
                        $vals = array('sat_day','sat_night');
                        break;
                    case 6:
                        //sunday
                        $day = 'Sun';
                        $vals = array('sun_day','sun_night');
                        break;
                }

                if ($day == $dayname)
                {
                    for($ohi = 0; $ohi <= 1; $ohi++)
                    {
                        $temp = explode(' - ', $this->OpeningHours[$this->SetLocationID][$vals[$ohi]]);
                        if ($temp[0] == 'Closed')
                        {
                            //closed skip
                            unset($openclass);
                        }
                        else
                        {
                            //we might be open
                            //check if number is greater than temp[0] and less than temp[1](+12 if pm)
                            $timeampm0 = substr($temp[0], -2);
                            $timeampm1 = substr($temp[1], -2);
                            $temp[0] = str_replace($timeampm0, '', $temp[0]);
                            $temp[1] = str_replace($timeampm1, '', $temp[1]);
                            if (($timeampm0 == 'pm') && ($temp[0] < 12))
                            {
                                $temp[0] += 12;

                            }
                            if (($timeampm1 == 'pm') || (($timeampm1 == 'am') && ($temp[1] <= 12)))
                            {
                                $temp[1] += 12;
                            }
                            if (($hour >= $temp[0]) && ($hour < $temp[1]))
                            {
                                //we are open
                                //set flag for td depending on the day/night
                                $todayval = $ohi;
                                $open = 'open';
                            }
                        }
                    }
                }
                else
                {
                    unset($ohclass);
                    unset($todayval);
                    unset($td1class);
                    unset($td2class);
                }

                if (isset($todayval))
                {
                    $ohclass=' class="success"';
                    /*switch ($todayval) {
                        case 0:
                            $td1class = ' class="success"';
                            $td2class = '';
                            break;
                        case 1:
                            $td1class = '';
                            $td2class = ' class="success"';
                    }*/
                }
                else
                {
                    unset($ohclass,$td1class,$td2class);
                }

                $ohstr .= '<tr' . $ohclass . '><td' . $ohclass . '>' . $day . '</td><td' . $td1class . '>' . $this->OpeningHours[$this->SetLocationID][$vals[0]] . '</td><td' . $td2class . '>' . $this->OpeningHours[$this->SetLocationID][$vals[1]] . '</td></tr>';
            }
            $this->setStoreVar('OHStatus', $open);
            $this->setStoreVar("OHCopy", '<table id="oh" class="table table-bordered table-condensed table-striped"><tr><th>&nbsp;</th><th>Day</th><th>Night</th></tr>' . $ohstr . '</table>');
        }

        function getTotalVisitorCount()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'total_visitor_count');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
            $rs = $DBA->selectQuery(DBSTORELOCATIONTABLE, $fields, $idfields, $idvals);
            
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setStoreVar('TVC', $res['total_visitor_count']);
                }
            }
            unset($DBA, $fields, $idfields, $idvals, $rs, $res);
        }

        function updateVisitorLog($userid, $major=null)
        {
            if (($userid != $this->OwnerID) && (!isset($major)))
            {
                $id = getMaxID(DBVISITORLOGTABLE);
                $DBA = new DatabaseInterface();
                $fields = array(0 =>'id', 1 => 'store_id', 2 => 'location_id', 3 => 'user_id', 4 => 'visitdate');
                $fieldvals = array(0 => $id, 1 => $this->ID, 2 => $this->SetLocationID, 3 => $userid, 4 => time());
                $rs = $DBA->insertQuery(DBVISITORLOGTABLE, $fields, $fieldvals);
                unset($rs, $fields, $fieldvals, $DBA);

                $this->getTotalVisitorCount();
                $count = $this->TVC + 1;
                $DBA = new DatabaseInterface();
                $fields = array(0 => 'total_visitor_count');
                $fieldvals = array(0 => $count);
                $idfields = array(0 => 'store_id', 1 => 'location_id');
                $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
                $rs = $DBA->updateQuery(DBSTORELOCATIONTABLE, $fields, $fieldvals, $idfields, $idvals);

                unset($fields, $fieldvals, $idfields, $idvals, $DBA, $rs);
            }
        }

        function getBasicReports()
        {
            //number of vistors
            /*$DBA = new DatabaseInterface();
            $fields = array(0 => 'count(id) as total');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

            $rs = $DBA->selectQuery(DBVISITORLOGTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $totvisitors = $res['total'];
                }
            }
            unset($DBA, $res, $rs, $fields, $idfields, $idvals);*/
            $this->getTotalVisitorCount();
            $totvisitors = $this->TVC;

            //number of favourite records

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'count(user_id) as total');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

            $rs = $DBA->selectQuery(DBUSERFAVOURITESTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $totfaves = $res['total'];
                }
            }
            unset($DBA, $res, $rs, $fields, $idfields, $idvals);


            unset($temphigh);
            $ocount = 0;
            $runtot = 0;
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);

            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $tempO = new Order();
                    $tempO->getOrder($res['id']);
                    $tempO->getTotalPrice();
                    if ((!isset($temphigh)) || ($temphigh <= $tempO->TotalPrice))
                    {
                        $temphigh = $tempO->TotalPrice;
                    }
                    $runtot += $tempO->TotalPrice;
                    $ocount += 1;
                    unset($tempO);
                }
            }
            else
            {
                $temphigh = 0;
            }

            $temphigh = number_format($temphigh, 2);
            if (($ocount > 0) && ($runtot > 0))
            {
                $avg = number_format($runtot/$ocount, 2);
            }

            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            //ratings
            $this->getOverallRating();
            //reviews
            $this->getReviews();

            //offers
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'count(dbid) as totoffers');
            $idfields = array(0 => 'store_id', 1 => 'type');
            $idvals = array(0 => $this->ID, 1 => 'Offer');
            
            $rs = $DBA->selectQuery(DBDOWNLOADDETAILSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $totdls = $res['totoffers'];
                    if ($totdls == "")
                    {
                        $totdls = 0;
                    }
                }
            }
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);

            $statisticsarray = array('times_rated' => $this->Locations[$this->SetLocationID]['rating_count'], 'average_rating' => $this->Locations[$this->SetLocationID]['overall_rating'], 'number_of_reviews' => count($this->Reviews), 'total_offer_downloads' => $totdls);
            //average order value
            $visitorarray = array('total_visitors' => $totvisitors, 'total_times_favourited' => $totfaves);
            $orderarray = array('total_orders' => $ocount, 'total_orders' => $ocount, 'highest_order_value' => $temphigh, 'average_order_value' => $avg);

            return(array('visitors' => $visitorarray, 'orders' => $orderarray, 'statistics' => $statisticsarray));
        }

        function getOrders()
        {
            $all = $this->getAllOrders();
            $recent = $this->getRecentOrders();
            $active = $this->getUnfinishedOrders();

            $orders = array('all' => $all, 'recent' => $recent, 'active' => $active);
            $this->setStoreVar('Orders', $orders);
        }

        function getAllOrders()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
            $orderby = 'time desc';

            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals, $orderby);

            if ((!isset($rs[2])) && ($rs[1] >=1 ))
            {
                $temp = array();
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }
            else
            {
                $temp = array(0 => 'No orders');
            }
            unset($rs, $fields, $idfields, $idvals, $res, $DBA);

            return ($temp);
        }

        function getRecentOrders($limit = 5)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'store_id', 1 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID);
            $limit = '0, ' . $limit;
            $orderby = 'time desc';
            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals, $orderby, $limit);

            if ((!isset($rs[2])) && ($rs[1] >=1 ))
            {
                $temp = array();
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }
            else
            {
                $temp = array(0 => 'No orders');
            }
            unset($rs, $fields, $idfields, $idvals, $res, $DBA);

            return ($temp);
        }

        function getUnfinishedOrders ()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'store_id', 1 => 'location_id', 2 => 'status');
            $idvals = array(0 => $this->ID, 1 => $this->SetLocationID,  2 => '!F');
            $orderby = 'id desc';

            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals, $orderby);

            if ((!isset($rs[2])) && ($rs[1] >=1 ))
            {
                $temp = array();
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }
            else
            {
                $temp = array(0 => 'No orders');
            }
            unset($rs, $fields, $idfields, $idvals, $res, $DBA);

            return ($temp);
        }
    }
?>
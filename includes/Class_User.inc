<?php
    abstract Class User {
        var $ID,
        $Username,
        $Title,
        $FirstName,
        $LastName,
        $Password,
        $AuthLevel,
        $ContactEmail,
        $ContactTelephone,
        $NewsletterSubscriber,
        $Optout,
        $DOB,
        $Area,
        $StoreOwner,
        $Long,
        $Lat,
        $Avatar,
        $AvatarFileLocation,
        $AvatarImageString,
        $FormattedAddress,
        $Mobile,
        $FormattedEmail,
        $FormattedPrintAddress,
        $Orders;

        function User() {
        }

        function setUserVar ($varname,$varvalue) {
            $this->$varname = $varvalue;
        }

        function getUser() {
            //get the users details
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'username', 1 => 'title', 2 => 'first_name', 3 => 'last_name', 4 => 'password', 5 => 'contactemail', 6 => 'contacttelephone', 7 => 'newsletter', 8 =>'pcode', 9 => 'store_owner', 10 => 'house', 11 => 'street', 12 => 'town', 13 => 'county', 14 => 'geolong', 15 => 'geolat', 16 => 'avatar', 17 => 'mobile', 18 => 'optout');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $this->ID);
            $rs = $DBA->selectQuery(DBUSERTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setUserVar('Username', $res['username']);
                    $this->setUserVar('Title', $res['title']);
                    $this->setUserVar('FirstName', $res['first_name']);
                    $this->setUserVar('LastName', $res['last_name']);
                    $this->setUserVar('ContactEmail', $res['contactemail']);
                    $this->setUserVar('ContactTelephone', $res['contacttelephone']);
                    $this->setUserVar('NewsletterSubscriber', $res['newsletter']);
                    $this->setUserVar('Area', $res['pcode']);
                    $this->setUserVar('StoreOwner', $res['store_owner']);
                    $this->setUserVar('House', $res['house']);
                    $this->setUserVar('Street', $res['street']);
                    $this->setUserVar('Town', $res['town']);
                    $this->setUserVar('County', $res['county']);
                    $this->setUserVar('PCode', $res['pcode']);
                    $this->setUserVar('Long', $res['geolong']);
                    $this->setUserVar('Lat', $res['geolat']);
                    $this->setUserVar('Avatar', $res['avatar']);
                    $this->setUserVar('Mobile', $res['mobile']);
                    $this->setUserVar('DMOptOut', $res['optout']);
                    $this->formatAddress();
                    $this->formatEmail();
                    if ($this->Lat == '')
                    {
                        $this->GeoLocate();
                    }
                }
            }
        }

        function getAvatar()
        {
            if ($this->Avatar == '')
            {
                $this->setUserVar('AvatarFileLocation', DEFAULT_USER_AVATAR);
                $this->setUserVar('AvatarFileLocation', "http://www.gravatar.com/avatar/" . md5(strtolower(trim($this->ContactEmail))) . "?d=" . urlencode($this->AvatarFileLocation) . "&s=128");
            }
            else
            {
                $this->setUserVar('AvatarFileLocation', USER_AVATAR_PATH . $this->Avatar);
            }
            $this->setUserVar('AvatarImageString', '<img src="' . $this->AvatarFileLocation . '" alt="' . $this->UserName . '" />');
        }

        function formatAddress($type = null)
        {
            $fa = $this->House . ' ';
            $fa .= $this->Street;
            if ($type=='print')
            {
                $fa .= ',\n';
            }
            else
            {
                $fa .= ',<br />';
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
            $fa .= $this->County;
            $fa .= '<br />' . $this->PCode;
            if ($type=='print')
            {
                $this->setUserVar('FormattedPrintAddress', $fa);
            }
            else
            {
                $this->setUserVar('FormattedAddress', $fa);
            }
        }

        function formatEmail()
        {
            if (strlen($this->ContactEmail) > 50)
            {
                $exp = explode('@', $this->ContactEmail);
                $fa = $exp[0] . '@<br />' . $exp[1];
            }
            else
            {
                $fa = $this->ContactEmail;
            }
            $this->setUserVar('FormattedEmail', $fa);
        }

        function updatePassword()
        {
            $DBA = new DatabaseInterface();
            $field = array(0 => 'password');
            $fieldval = array(0 => $this->Password);
            $idfield = 'id';
            $idval = $this->ID;

            $rs = $DBA->updateQuery(DBUSERTABLE, $field, $fieldval, $idfield, $idval);
        }
    }


    Class Supplier extends User {
        var $StoreID,
            $StoresList;

        function Supplier() {
        }

        function setSupplierVar($varname, $varvalue)
        {
            $this->$varname = $varvalue;
        }

        function getSupplier($ID = null)
        {
            //gets supplier information from database and fills object

        }
        function registerUser()
        {
            //Creates new user and saves info
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'title', 1 => 'first_name', 2 => 'last_name', 3 => 'contacttelephone', 4 => 'contactemail', 5 => 'password', 6 => 'username', 7 => 'dob', 8 => 'house', 9 => 'street', 10 => 'town', 11 => 'county', 12 => 'pcode', 13 => 'optout', 14  => 'newsletter', 15 => 'auth_level', 16 => 'store_owner');
            $fieldvals = array(0 => $this->Title, 1 => $this->FirstName, 2 => $this->LastName, 3 => $this->ContactTelephone, 4 => $this->ContactEmail, 5 => $this->Password, 6 => $this->Username, 7 => $this->DOB, 8 => $this->House, 9 => $this->Street, 10 => $this->Town, 11 => $this->County, 12 => $this->PCode, 13 => $this->Optout, 14 => $this->NewsletterSubscriber, 15 => $this->AuthLevel, 16 => 1);
            $rs = $DBA->insertQuery(DBUSERTABLE, $fields, $fieldvals);
            if ($rs == 1)
            {
                $this->RegistrationSuccess = 1;
            }
            if (isset($this->PCode))
            {
                $this->geoLocate();
            }
        }

        function getSupplierStores($override = null)
        {
            $DBA = new DatabaseInterface();
            if (!$override)
            {
                $fields = array(0 => 'id');
                $idfields = array(0 => 'owner_id');
                $idvals = array(0 => $this->ID);
                $table = DBSTORETABLE;
            }
            else
            {
                $fields = array(0 => 'distinct(store_id) as id');
                $idfields = array(0 => 'imei');
                $idvals = array(0 => '!null');
                $table = DBSTORELOCATIONTABLE;
            }
            $rs = $DBA->selectQuery($table, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1]>=1 ))
            {
                $return = array();
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($return, $res);
                }
            }
            else
            {
                unset($return);
            }
            $this->setUserVar('StoresList', $return);
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);
        }

        function getRecentOrders($store_id=null)
        {

        }

        function deleteSupplier()
        {
            //removes supplier from the system
        }
    }


    Class Customer extends User {
        var $House,
        $Street,
        $Town,
        $County,
        $PCode,
        $BillingAddress1,
        $BillingAddress2,
        $BillingAddress3,
        $BillingAddress4,
        $BillingPCode,
        $SavedSearches,
        $SavedOrders,
        $SavedCardID1,
        $SavedCardID2,
        $SavedCardID3,
        $Reviews,
        $ReviewCount,
        $Favourites,
        $DistanceToStore;

        function Customer() {
        }

        function setCustomerVar($varname, $varvalue)
        {
            $this->$varname = $varvalue;
        }

        function createNewCustomer($vals,$dels = null) {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $idfields = array(0 => 'contactemail',1 => 'first_name', 2 => 'last_name');
            $idvals = array(0 => $vals['email'],1 => $vals['firstname'], 2 => $vals['surname']);
            $rs = $DBA->selectQuery(DBUSERTABLE, $fields, $idfields,$idvals);

            if ($rs[1] >=1)
            {
                //there is a problem but just do nothing...they can try log on fail if needs be and reset password
                $this->ID = $res['id'];
            }
            else
            {
                //there is no record
                //so insert
                $fields = array(0 => 'first_name', 1 => 'last_name', 2 => 'contacttelephone', 3 => 'contactemail', 4 => 'auth_level');
                $fieldvals = array(0 => $vals['firstname'], 1=> $vals['surname'], 2 => $vals['telno'], 3 => $vals['email'], 4 => 3);
                if (count($dels) >= 1)
                {
                    //we have delivery details we can add them in they can change them later on
                    array_push($fields, 'house','street','town','county','pcode');
                    array_push($fieldvals, $dels['house'], $dels['street'], $dels['town'], $dels['county'], $dels['pcode']);
                }

                if ($vals['account'] == 1)
                {
                    $username = $vals['firstname'] . '.' . $vals['surname'];
                    array_push($fields, 'password','username');
                    array_push($fieldvals, $vals['password'],$username);
                }

                $DBA = new DatabaseInterface();
                $rs = $DBA->insertQuery(DBUSERTABLE, $fields, $fieldvals);

                unset($rs, $fields, $fieldvals, $DBA);

                $DBA = new DatabaseInterface();
                $fields = array(0 => 'max(id) as id');
                $rs = $DBA->selectQuery(DBUSERTABLE, $fields);
                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        $this->ID = $res['id'];
                    }
                }
                unset($rs, $fields, $res, $DBA);
            }   
        }

        function registerUser()
        {
            //Creates new user and saves info
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'title', 1 => 'first_name', 2 => 'last_name', 3 => 'contacttelephone', 4 => 'contactemail', 5 => 'password', 6 => 'username', 7 => 'dob', 8 => 'house', 9 => 'street', 10 => 'town', 11 => 'county', 12 => 'pcode', 13 => 'optout', 14  => 'newsletter', 15 => 'auth_level');
            $fieldvals = array(0 => $this->Title, 1 => $this->FirstName, 2 => $this->LastName, 3 => $this->ContactTelephone, 4 => $this->ContactEmail, 5 => $this->Password, 6 => $this->Username, 7 => $this->DOB, 8 => $this->House, 9 => $this->Street, 10 => $this->Town, 11 => $this->County, 12 => $this->PCode, 13 => $this->Optout, 14 => $this->NewsletterSubscriber, 15 => $this->AuthLevel);
            $rs = $DBA->insertQuery(DBUSERTABLE, $fields, $fieldvals);
            if ($rs == 1)
            {
                $this->RegistrationSuccess = 1;
            }
            if (isset($this->PCode))
            {
                $this->geoLocate();
            }
            unset($DBA, $rs, $fields, $fieldvals);
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $idfields = array(0 => 'username');
            $idvals = array(0 => $this->Username);
            $rs = $DBA->selectQuery(DBUSERTABLE, $fields, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setUserVar('ID', $res['id']);
                }
            }
            unset($DBA, $rs, $fields, $idfields, $idvals);
        }

        function updateUser($change_address = 0)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'title', 1 => 'first_name', 2 => 'last_name', 3 => 'contacttelephone', 4 => 'contactemail', 5 => 'password', 6 => 'username', 7 => 'dob', 8 => 'house', 9 => 'street', 10 => 'town', 11 => 'county', 12 => 'pcode', 13 => 'optout', 14  => 'newsletter', 15 => 'auth_level');
            $fieldvals = array(0 => $this->Title, 1 => $this->FirstName, 2 => $this->LastName, 3 => $this->ContactTelephone, 4 => $this->ContactEmail, 5 => $this->Password, 6 => $this->Username, 7 => $this->DOB, 8 => $this->House, 9 => $this->Street, 10 => $this->Town, 11 => $this->County, 12 => $this->PCode, 13 => $this->Optout, 14 => $this->NewsletterSubscriber, 15 => $this->AuthLevel);
            $idfields = 'id';
            $idvals = $this->ID;
            $rs = $DBA->updateQuery(DBUSERTABLE, $fields, $fieldvals, $idfields, $idvals);
            if ($rs == 1)
            {
                $this->RegistrationSuccess = 1;
            }
            if ((isset($this->PCode)) && ($change_address == 1))
            {
                $this->geoLocate();
            }
        }

        function getRecentSearches()
        {
            //returns an array of values
        }

        function getOrders()
        {
            $all = $this->getAllOrders();
            $recent = $this->getRecentOrders();
            $active = $this->getUnfinishedOrders();

            $orders = array('all' => $all, 'recent' => $recent, 'active' => $active);
            $this->setUserVar('Orders', $orders);
        }

        function getAllOrders()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'store_id', 2 => 'menu_id', 3 => 'location_id', 4 => 'reference', 5 => 'deliverytype', 6 => 'status', 7 => 'ready_time', 8 => 'time', 9 => 'archived');
            $idfields = array(0 => 'user_id');
            $idvals = array(0 => $this->ID);

            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals);

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
            $diff = time(); //now minus 1 month
            $diff-= 2592000;
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'store_id', 2 => 'menu_id', 3 => 'location_id', 4 => 'reference', 5 => 'deliverytype', 6 => 'status', 7 => 'ready_time', 8 => 'time', 9 => 'archived');
            $idfields = array(0 => 'user_id', 1 => 'time');
            $idvals = array(0 => $this->ID, 1 => '>' . $diff);
            $order = 'time desc';
            $limit = '0, ' . $limit;

            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals, $order, $limit);

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
            $fields = array(0 => 'id', 1 => 'store_id', 2 => 'menu_id', 3 => 'location_id', 4 => 'reference', 5 => 'deliverytype', 6 => 'status', 7 => 'ready_time', 8 => 'time', 9 => 'archived');
            $idfields = array(0 => 'user_id', 1 => 'status');
            $idvals = array(0 => $this->ID, 1 => '!F');
            $orderby = 'id desc';

            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals, $orderby);

            $counter = 0;
            if ((!isset($rs[2])) && ($rs[1] >=1))
            {
                $temp = array();
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if ($res['status'] == 'X')
                    {
                        if ($res['archived'] != "1")
                        {
                            $counter++;
                            array_push($temp, $res);
                        }
                    }
                    else
                    {
                        $counter++;
                        array_push($temp, $res);
                    }
                }
            }
            if ($counter == 0)
            {
                $temp = array(0 => 'No orders');
            }
            unset($rs, $fields, $idfields, $idvals, $res, $DBA);

            return ($temp);
        }

        function getSavedSearches()
        {
            //returns an array of values
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'search_string', 1 => 'id');
            $idfields = array(0 => 'user_id');
            $idvals = array(0 => $this->ID);
            $orderby = 'id desc';

            $rs = $DBA->selectQuery(DBSAVEDSEARCHESTABLE, $fields, $idfields, $idvals, $orderby);
            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                $temp = array();
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }
            else
            {
                $temp = array(0 => 'No Saved Searches');
            }

            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            $this->setUserVar('SavedSearches', $temp);
        }

        function removeSavedSearch($id)
        {
            $DBA = new DatabaseInterface();
            $sql = "delete from " . DBSAVEDSEARCHESTABLE . " where id=" . $id;
            $rs = $DBA->runQuery($sql);
        }

        function getReviews()
        {
            $temp = array();
            //returns an array of values
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'store_id', 2 => 'content', 3 => 'review_date', 4 => 'location_id');
            $idfields = array(0 => 'user_id');
            $idvals = array(0 => $this->ID);
            $orderby = 'review_date desc';

            $rs = $DBA->selectQuery(DBREVIEWSTABLE, $fields, $idfields, $idvals, $orderby);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }

            $this->setUserVar('Reviews', $temp);
        }

        function getRatings($store_id=null,$location_id=null,$associated_review_id=null)
        {
            //returns an array of values
            $temp = array();
            $temp2 = array();
            $counter = 0;
            $stored = '';
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'rating', 1 => 'rating_type_id', 2 => 'store_id', 3 => 'location_id');
            $idfields = array(0 => 'user_id');
            $idvals = array(0 => $this->ID);
            if (isset($store_id))
            {
                array_push($idfields, 'store_id');
                array_push($idvals, $store_id);
            }
            if (isset($location_id))
            {
                array_push($idfields, 'location_id');
                array_push($idvals, $location_id);
            }
            if (isset($associated_review_id))
            {
                array_push($idfields, 'associated_review_id');
                array_push($idvals, $associated_review_id);
            }
            $orderby = 'store_id, location_id, rating_type_id';
            unset($groupby);
            $rs = $DBA->selectQuery(DBRATINGSTABLE, $fields, $idfields, $idvals, $orderby);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if (!isset($store_id))
                    {
                        if (($res['store_id'] == $stored) || ($stored == ''))
                        {
                            //add to temp2
                            array_push($temp2, $res);
                        }
                        else
                        {
                            array_push($temp, $temp2);
                            unset($temp2);
                            $temp2 = array();
                            array_push($temp2, $res);
                            $open = 1;
                        }
                        $stored = $res['store_id'];
                    }
                    else
                    {
                        array_push($temp, $res);
                    }
                    $counter += 1;
                }
                if (($open == 1) && (!isset($store_id)))
                {
                    array_push($temp, $temp2);
                }
            }

            $this->Ratings = $temp;
            $this->RatingCount = $counter/3;
        }

        function getSavedOrders()
        {
            //returns an array of orders
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'reference');
            $idfields = array(0 => 'user_id');
            $idvals = array(0 => $this->ID);
            $orderby = 'id desc';

            $rs = $DBA->selectQuery(DBUSERSAVEDORDERSTABLE, $fields, $idfields, $idvals, $orderby);

            $refarray = array();
            $counter = 0;

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $refarray[$counter] = $res['reference'];
                    $counter++;
                }
            }

            unset($DBA, $fields, $idfields, $idvals, $rs, $res);
            $this->setUserVar('SavedOrders', $refarray);
        }

        function checkSavedOrder($ref)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => '1');
            $idfields = array(0 => 'reference');
            $idvals = array(0 => $ref);

            $rs = $DBA->selectQuery(DBUSERSAVEDORDERSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >=1 ))
            {
                unset($rs, $DBA, $fields, $idfields, $idvals);
                return true;
            }
            else
            {
                unset($rs, $DBA, $fields, $idfields, $idvals);
                return false;
            }
        }

        function changeSavedOrderStatus($ref, $type)
        {
            $DBA = new DatabaseInterface();
            switch ($type) {
                case 'Add':
                    $id= getmaxid(DBUSERSAVEDORDERSTABLE);
                    $fields = array(0 => 'id', 1 => 'user_id', 2 => 'reference');
                    $fieldvals = array(0 => $id, 1 => $this->ID, 2 => $ref);
                    $rs = $DBA->insertQuery(DBUSERSAVEDORDERSTABLE, $fields, $fieldvals);
                    session_push('<p>Order added to filing cabinet</p>');
                    break;
                case 'Remove':
                    $sql = 'delete from ' . DBUSERSAVEDORDERSTABLE . ' where user_id="' . $this->ID . '" and reference="' . $ref .'"';
                    $DBA->runQuery($sql);
                    session_push('<p>Order removed from filing cabinet</p>');
                    break;
            }
            unset($DBA, $id, $fields, $fieldvals, $rs, $sql);
        }

        function updateUserPayment($cardid)
        {
            //updates card details returns true if successful
            if (!isset($cardid))                                                                                                     
            {
                error_push('Must choose a card');
                return false;
            }
        }

        function geoLocate()
        {
              $key = GAPI_KEY;
             // Loop through each row, submit HTTP request, output coordinates
             $mapaddress = urlencode($this->House . '+' . $this->Street . '+' . $this->Town . '+' . $this->County . '+' . $this->PCode);

             // Desired address
             $url = "http://maps.google.com/maps/geo?q=$mapaddress&output=xml&key=$key";

             // Retrieve the URL contents
             $page = file_get_contents($url);

             // Parse the returned XML file
             $xml = new SimpleXMLElement($page);

             // Parse the coordinate string
             list($longitude, $latitude, $altitude) = explode(",", $xml->Response->Placemark->Point->coordinates);

             // Output the coordinates
             $fields = array(0 => 'geolat', 1 => 'geolong');
             $fieldvals = array(0 => $latitude, 1 => $longitude);
             $idfield = 'id';
             $idfieldvals = $this->ID;

             $DBA = new DatabaseInterface();
             $rs = $DBA->updateQuery(DBUSERTABLE, $fields, $fieldvals, $idfield, $idfieldvals);

             unset($DBA, $rs, $fields, $fieldvals, $idfield, $idfieldvals);

             $this->setUserVar('Lat', $latitude);
             $this->setUserVar('Long', $longitude);
        }

        function createResetURL()
        {
             $hash = create_random();
             return (SITE_PATH . RESET_PASSWORD_PAGE . $hash);
        }

        function getFavourites()
        {
            $temp = array();

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'store_id', 1 => 'location_id');
            $idfields = array(0 => 'user_id');
            $idvals = array(0 => $this->ID);

            $rs = $DBA->selectQuery(DBUSERFAVOURITESTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }

            $this->setUserVar('Favourites', $temp);
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);
        }

        function checkFavourite($store_id, $location_id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $idfields = array(0 => 'user_id', 1 => 'store_id', 2 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $store_id, 2 => $location_id);

            $rs = $DBA->selectQuery(DBUSERFAVOURITESTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        function Favourite($store, $ftype = 'Add', $location_id)
        {
            $DBA = new DatabaseInterface();
            if ($ftype == 'Remove')
            {
                $sql = 'delete from ' . DBUSERFAVOURITESTABLE . ' where user_id = ' . $this->ID . ' and store_id = ' . $store . ' and location_id = ' . $location_id;
                $DBA->runQuery($sql);
            }
            else
            {
                $id= getmaxid(DBUSERFAVOURITESTABLE);
                $fields = array(0 => 'id', 1 => 'user_id', 2 => 'store_id', 3 => 'location_id');
                $fieldvals = array(0 => $id, 1 => $this->ID, 2 => $store, 3 => $location_id);
                $rs = $DBA->insertQuery(DBUSERFAVOURITESTABLE, $fields, $fieldvals);
            }
            unset($rs, $fields, $fieldvals, $id, $sql, $DBA);
        }

        function updateContactDetails()
        {
            $fields = array(0 => 'first_name', 1 => 'last_name', 2 => 'house', 3 => 'street', 4 => 'town', 5 => 'county', 6 => 'pcode', 7 => 'contacttelephone', 8 => 'mobile', 9 => 'contactemail');
            $fieldvals = array(0 => $this->FirstName, 1 => $this->LastName, 2 => $this->House, 3 => $this->Street, 4 => $this->Town, 5 => $this->County, 6 => $this->PCode, 7 => $this->ContactTelephone, 8 => $this->Mobile, 9 => $this->ContactEmail);
            $idfield = 'id';
            $idval = $this->ID;

            $DBA = new DatabaseInterface();
            $rs = $DBA->updateQuery(DBUSERTABLE, $fields, $fieldvals, $idfield, $idval);
            unset($rs, $fields, $fieldvals, $idfield, $idval, $DBA);
        }

        function calculateDistanceToStore($storecoords) {
            $coords = array('lat' => $this->Lat, 'long' => $this->Long);
            $distance = distance_apart($coords, $storecoords);
            $distance = round($distance, 2);
            $this->setUserVar('DistanceToStore', $distance);
        }
    }
?>
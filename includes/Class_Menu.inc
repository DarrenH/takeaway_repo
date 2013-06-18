<?php
    Class Menu {
        var $ID,
        $StoreID,
        $LocationID,
        $ReferenceName,
        $Description,
        $Items,
        $Timed,
        $ApplicableTimes,
        $Status,
        $MenuStr,
        $Categories;

        function Menu(){
        }

        function setMenuVar($varname,$varvalue)
        {
            $this->$varname = $varvalue;
        }

        function getMenu()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description', 2 => 'timed', 3 => 'status');
            $idfields = array(0 => 'id', 1 => 'store_id', 2 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->StoreID, 2 => $this->LocationID);

            $rs = $DBA->selectQuery(DBOLMENUTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] >=1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setMenuVar('ReferenceName', stripslashes($res['name']));
                    $this->setMenuVar('Description', $res['description']);
                    $this->setMenuVar('Timed', $res['timed']);
                    $this->setMenuVar('Status', $res['status']);
                    $this->checkMenuStatus();
                    $this->setMenuVar('Categories', $this->getCategories(1));
                }
            }
            else
            {
                $this->setMenuVar('ReferenceName', "none");
            }
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);
        }

        function createMenu()
        {

        }

        function updateMenu()
        {

        }

        function deleteMenu()
        {
            $success = $this->deleteAllItems();
            if ($success)
            {
                unset($success);
                $success = $this->deleteAllCategories();
                if ($success)
                {
                    unset($success);
                    $success = $this->deleteHeader();
                }
            }

            if (!$success)
            {
                return("Error");
            }
            else
            {
                return(true);
            }
        }

        function deleteAllItems($category = null)
        {
            $DBA = new DatabaseInterface();
            if (isset($catgeory))
            {
                $idfields = array(0 => 'menu_id', 1 => 'category');
                $idvals = array(0 => $this->ID, 1 => $category);
            }
            else
            {
                $idfields = 'menu_id';
                $idvals = $this->ID;
            }

            $rs = $DBA->deleteRecord(DBOLMENUITEMSTABLE, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                $ret = true;
            }
            else
            {
                $ret = false;
            }
            unset($DBA, $idfield, $idval, $rs);
            return ($ret);
        }

        function deleteAllCategories($categoryid = null)
        {
            $DBA = new DatabaseInterface();
            if (isset($categoryid))
            {
                $idfields = array(0 => 'id', 1 => 'menu_id');
                $idvals = array(0 => $categoryid, 1 => $this->ID);
            }
            else
            {
                $idfields = 'menu_id';
                $idvals = $this->ID;
            }
            $rs = $DBA->deleteRecord(DBOLMENUCATEGORYTABLE, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                $ret = true;
            }
            else
            {
                $ret = false;
            }
            unset($DBA, $idfield, $idval, $rs);
            return ($ret);
        }

        function deleteHeader()
        {
            $DBA = new DatabaseInterface();
            $idfields = array(0 => 'id', 1 => 'store_id', 2 => 'location_id');
            $idvals = array(0 => $this->ID, 1 => $this->StoreID, 2 => $this->LocationID);
            $rs = $DBA->deleteRecord(DBOLMENUTABLE, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                $ret = true;
            }
            else
            {
                $ret = false;
            }
            unset($DBA, $idfield, $idval, $rs);
            return ($ret);
        }

        function deleteCategory($categoryid)
        {
            $success = $this->deleteAllItems($categoryid);
            if ($success)
            {
                unset($success);
                $success = $this->deleteAllCategories($categoryid);
            }

            if (!$success)
            {
                return("Error");
            }
            else
            {
                return(true);
            }
        }

        function addItemtoMenu($item_id)
        {

        }

        function removeItemfromMenu($item_id)
        {

        }

        function getMasterMenuItems($storeid, $locationid, $storemenuid, $storecatid, $cuisine_type = null)
        {
            //get categories by cuisine type
            $fields = array(0 => 'id', 1 => 'name');
            $idfields = array(0 => 'cuisine_type_id');
            $idvals = array(0 => $cuisine_type[0]['id']);

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBOLMASTERMENUCATEGORYTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                //prepare display of category header and lists of items.
                while ($res = mysql_fetch_array($rs[0]))
                {
                    //check number of items in category
                    if($this->getNumberofMasterCategoryItems($res['id']) >= 1)
                    {
                        //make li record with $catname as id and text
                        $lis .= '<li><a href="#' . str_replace(' ','_',strtolower($res['name'])) . '" title="View ' . ucwords($res['name']) . ' Items">' . ucwords($res['name']) . '</a></li>';
                        $html .= '<div id="' . str_replace(' ', '_', strtolower($res['name'])) . '">';
                        $html .= '<a class="nav" name="' . strtolower($res['name']) . '"></a>';
                        $html .= '<ul class="menu-items">';
                        $html .= '<li class="menuitem heads"><span class="menuname">Product Name</span><span class="cost"></span><span class="form-column-header"></span></li>';

                        //grab items and loop adding them to list till all done.
                        $itemlist = array();

                        $DBA2 = new DatabaseInterface();
                        $fields2 = array(0 => 'name', 1 => 'description', 3 => 'category', 4 => 'id');
                        $idfields2 = array(0 => 'category');
                        $idvals2 = array(0 => $res['id']);
                        $orderby2 = 'id';

                        $rs2 = $DBA2->selectQuery(DBOLMASTERMENUITEMSTABLE, $fields2, $idfields2, $idvals2, $orderby2);

                        if (!isset($rs2[2]))
                        {
                            while ($res2 = mysql_fetch_array($rs2[0]))
                            {
                                unset($exists);
                                $DBA3 = new DatabaseInterface();
                                $fields3 = array(0 => 'count(id) as ex');
                                $idfields3 = array(0 => 'menu_id', 1 => 'master_menu_id');
                                $idvals3 = array(0 => $storemenuid, 1 => $res2['id']);

                                $rs3 = $DBA2->selectQuery(DBOLMENUITEMSTABLE, $fields3, $idfields3, $idvals3);

                                unset($extracss);
                                unset($exists);

                                if (!isset($rs3[2]))
                                {
                                    while ($res3 = mysql_fetch_array($rs3[0]))
                                    {
                                        if ($res3['ex'] != 0)
                                        {
                                            //this item has been placed on the menu already but we could still add it and change stuff.
                                            $exists = 1;

                                            unset($DBA3, $fields3, $idfields3, $idvals3, $rs3);
                                            $DBA3 = new DatabaseInterface();
                                            $fields3 = array(0 => 'id', 1 => 'name', 2 => 'description');
                                            $idfields3 = array(0 => 'menu_id', 1 => 'master_menu_id');
                                            $idvals3 = array(0 => $storemenuid, 1 => $res2['id']);

                                            $existhtml = '<span class="qtyform"><img src="' . SITE_PATH . 'images/onmenu.png" alt="This item already exists on your menu, you may have changed it totally or wish to use it as a basis for another item, so please add it again if you wish." />';
                                            $existhtml .= '<span class="existpop">';
                                            $existhtml .= '<p>The following products on your menu were originally based on this item.</p>';
                                            $rs3 = $DBA3->selectQuery(DBOLMENUITEMSTABLE, $fields3, $idfields3, $idvals3);
                                            while ($res3 = mysql_fetch_array($rs3[0]))
                                            {
                                                //add individual stuff for this item
                                                $existhtml .= '<ul>';
                                                $existhtml .= '<li>Product Name: ' . $res3['name'] . '</li>';
                                                $existhtml .= '<li class="last">Description: ' . $res3['description'] . '</li>';
                                                $existhtml .= '</ul>';
                                            }
                                            $existhtml .= '</span></span>';
                                        }
                                    }
                                }
                                unset($DBA3, $fields3, $idfields3, $idvals3, $rs3);
                                $html .= '<li class="menuitem' . $extracss . ' mit' . $res2['id'] . '"><span class="mleft"><span class="menuname">' . $res2['name'] . '</span>';
                                array_push($itemlist, $res2);
            //                    $html .= '<span class="hld">';
                                if (isset($exists))
                                {
                                    $html .= $existhtml;
                                    unset($existhtml);
                                }
                                else
                                {
                                    //$html .= '<form action="' . SITE_PATH . 'add-to-cart/' . $this->StoreID . '/' . $this->ID . '/" id="' . $counter . '" name="' . $counter . '" method="post" ><input class="smqty" type="text" name="qty" value="" /><input type="submit" name="sub" value="Add" class="pointer misub addone" /><input type="hidden" name="itemid" value="' . $res['id'] . '" /><input type="hidden" name="catid" value="' . $catid . '" /><input type="hidden" name="locationid" value="' . $this->LocationID . '" /></form>';
                                }
                                $html .= '<span class="description-full">' . $res2['description'] . '</span></span>';
            //                    $html .= '</span>';
                                $html .= '<span class="bskicons"><a class="addone" href="' . SITE_PATH . 'add-item-to-menu/' . $storeid . '/' . $storemenuid . '/' . $res2['id'] . '/' . $storecatid . '/' . $locationid . '/" title="Add 1 of these items to your cart"><img src="' . SITE_PATH . 'images/add-to-menu.png" title="Add this item to your menu" /></a><!--<a class="remone" href="' . SITE_PATH . 'remove-item-from-menu/' . $storeid . '/' . $storemenuid . '/' . $res2['id'] . '/' . $storecatid . '/' . $locationid . '/" title="Remove this item from your menu"><img src="' . SITE_PATH . 'images/cart-remove.gif" alt="Remove this item from your menu." /></a>--></span></li>';
                            }
                        }
                        $html .= '</ul>';
                        $html .= '<a class="nav" href="#top">Back to top</a>';
                        $html .= '</div>';
                    }
                }

                $finstr = '<div id="menuinner">';
                $finstr .= '<a name="top"></a>';
                $finstr .= '<ul>';
                $finstr .= $lis;
                $finstr .= '</ul>';
                $finstr .= $html;
                $finstr .= '</div>';
                $finstr .= '</div>';

                return ($finstr);

            }
            elseif ($rs[1] == 0)
            {
                //no categories therefore no menu
                return ('There are no items available for this cuisine type.');
            }
            else
            {
                //error
                return ('There was a problem trying to retrieve the items for this cuisine type. Please try again and if you continue to have problems please contact us.');
            }
        }

        function getMenuItems3($menucreate=false)
        {
            $itemlist = array(); $str ='';
            foreach ($this->Categories as $cat)
            {
                if ($cat['itemcount'] >= 1)
                {
                    $str .= '<div class="cathead" id="' . $cat['safe-name'] . '"><h3 itemprop="servesCuisine">' . $cat['name'] . '</h3>';
                    //$str .='<div id="item-headers"><div class="span3">Item</div><div class="span6">Description</div><div class="span1">Price</div><div class="span1 add">Add</div><div class="span1">&nbsp;</div></div>';
                    //get items for cat..

                    $DBA2 = new DatabaseInterface();
                    $fields2 = array(0 => 'name', 1 => 'description', 2 => 'ptype', 3 => 'cost', 4 => 'id');
                    $idfields2 = array(0 => 'menu_id', 1 => 'category');
                    $idvals2 = array(0 => $this->ID, 1 => $cat['id']);
                    $orderby2 = 'category, name, id';
                    
                    $rs2 = $DBA2->selectQuery(DBOLMENUITEMSTABLE, $fields2, $idfields2, $idvals2, $orderby2);

                    if ((!isset($rs2[2])) && ($rs2[1] != 0))
                    {
                        while ($res = mysql_fetch_array($rs2[0]))
                        {
                            if ($res['ptype'] == 'S')
                            {
                                $str .= '<div class="item-row row-fluid name-row">';
                                if ($res['description'] !== '')
                                {
                                    $str .= '<div class="span12 item-name">' . $res['name'] . '</div>';  
                                    $str .= '</div>';
                                    $str .= '<div class="item-row row-fluid">';
                                    $str .= '<div class="span9 item-desc">' . $res['description'] . '</div>';
                                }
                                else
                                {
                                    $str .= '<div class="span9 item-name">' . $res['name'] . '</div>';
                                }
                                $str .= '<div class="span1 item-cost">' . $res['cost'] . '</div>';
                                $str .= '<div class="span1 item-add button"><button type="submit" data-loading-text="..." class="btn btn-success btn-small" formaction="' . SITE_PATH . 'add-to-basket/' . $this->ID . '/' . $res['id'] . '/">ADD</button></div>';
                                $str .= '<div class="span1 hidden-phone">&nbsp;</div>';
                                $str .= '<div class="clearfix"></div>';
                                $str .= '</div>';
                            }
                            else
                            {
                                $str .= $this->getMultiprice($res['id'],$res['name'],$res['description']);
                            }

                        }
                    }    
                    $str .= '</div>';                
                    $str .= '<div class="clearfix"></div>';               
                }
            }
            $str .='<div class="clearfix"></div>';

            return $str;
        }

        function getMenuItems2($menucreate=false)
        {
            $tabslist = getMainTabs();
            $itemlist = array();
            if (count($tabslist) >= 1)
            {
                foreach ($tabslist as $tab)
                {
                    //make list for main tabs
                    $maintabstr .= '<li><a href="#' . str_replace("'", "", str_replace(' ', '_',str_replace('&', 'and', $tab['name']))) . '" alt="View categories and items for ' . ucwords($tab['name']) . '">' . ucwords($tab['name']) . '</a></li>';

                    $tabcontent .= '<div id ="' . str_replace("'", "", str_replace(' ', '_',str_replace('&', 'and', $tab['name']))) . '">';

                    //go through the main tabs to grab categories then items
                    $DBA = new DatabaseInterface();
                    $fields = array(0 => 'id', 1 => 'name', 2 => 'description');
                    $idfields = array(0 => 'menu_id', 1 => 'main_tab');
                    $idvals = array(0 => $this->ID, 1 => $tab['id']);
                    $orderby = 'name';

                    $rs = $DBA->selectQuery(DBOLMENUCATEGORYTABLE, $fields, $idfields, $idvals, $orderby);
                    
                    if ((!isset($rs[2])) && ($rs[1] != 0))
                    {
                        while ($res = mysql_fetch_array($rs[0]))
                        {
                            //ok so we now have all the categories under the main tab
                            $catlist .= '<li><a href="#' . str_replace(",", "", str_replace("&", "and", str_replace("'", "", str_replace(' ','_',strtolower($res['name']))))) . '" title="View ' . ucwords($res['name']) . ' Items">' . ucwords($res['name']) . '</a></li>';

                            //now get all of the items for this category
                            $DBA2 = new DatabaseInterface();
                            $fields2 = array(0 => 'name', 1 => 'description', 2 => 'ptype', 3 => 'cost', 4 => 'id');
                            $idfields2 = array(0 => 'menu_id', 1 => 'category');
                            $idvals2 = array(0 => $this->ID, 1 => $res['id']);
                            $orderby2 = 'category, id';

                            $rs2 = $DBA2->selectQuery(DBOLMENUITEMSTABLE, $fields2, $idfields2, $idvals2, $orderby2);

                            $items .= '<div id="' . str_replace(",", "", str_replace("&", "and", str_replace("'", "", str_replace(' ','_',strtolower($res['name']))))) . '">';
                            $items .= '<a class="nav" name="' . str_replace(",", "", str_replace("&", "and", str_replace("'", "", str_replace(' ','_',strtolower($res['name']))))) . '"></a>';
                            
                            if ((!isset($rs2[2])) && ($rs2[1] != 0))
                            {
                                $items .= '<ul class="menu-items">';
                                if ($res['description'] != "")
                                {
                                    $items .= '<li>' . $res['description'] . '</li>';
                                }
                                $items .= '<li class="menuitem heads"><span class="menuname">Product Name</span><span class="cost">Cost(&pound;)</span><span class="form-column-header">Quantity</span></li>';
                                $counter = 0;
                                while ($res2 = mysql_fetch_array($rs2[0]))
                                {
                                    //this is the start of an item
                                    if ($res2['ptype'] == 'S')
                                    {
                                        unset($qty_bought);

                                        if (isset($_SESSION['basket_id']))
                                        {
                                            $DBA3 = new DatabaseInterface();
                                            $fields3 = array(0 => 'number_of_items');
                                            $idfields3 = array(0 => 'basket_id', 1 => 'item_id');
                                            $idvals3 = array(0 => $_SESSION['basket_id'], 1 => $res2['id']);

                                            $rs3 = $DBA3->selectQuery(DBBASKETITEMSTABLE, $fields3, $idfields3, $idvals3);

                                            unset($extracss);
                                            if (!isset($rs3[2]))
                                            {
                                                while ($res3 = mysql_fetch_array($rs3[0]))
                                                {
                                                    $qty_bought += $res3['number_of_items'];
                                                    $extracss = ' selectedline';
                                                }
                                            }
                                            unset($DBA3, $fields3, $idfields3, $idvals3, $rs3);
                                        }

                                        $items .= '<li class="menuitem' . $extracss . ' mit' . $res2['id'] . '">';
                                        $items .= '<span class="menuname">' . $res2['name'] . '</span><span class="cost">' . $res2['cost'] . '</span>';
                                        $iarray = array('id' => $res2['id'], 'menu_id' => $this->ID, 'category' => $res['id'], 'name' => $res2['name'], 'ptype' => 'S', 'cost' => $res2['cost'], 'description' => $res2['description'], 'extras' => array());
                                        //array_push($itemlist, $res);

                                        if (isset($qty_bought))
                                        {
                                            $items .= '<span class="qtyform">' . $qty_bought . '</span>';
                                        }
                                        else
                                        {
                                            $items .= '<form action="' . SITE_PATH . 'add-to-cart/' . $this->StoreID . '/' . $this->ID . '/" id="' . $counter . '" name="' . $counter . '" method="post" ><input class="smqty" type="text" name="qty" value="" /><input type="submit" name="sub" value="ADD" class="pointer misub addone" /><input type="hidden" name="itemid" value="' . $res2['id'] . '" /><input type="hidden" name="catid" value="' . $res['id'] . '" /><input type="hidden" name="locationid" value="' . $this->LocationID . '" /></form>';
                                        }
                                        //$html .= '</span>';
                                        $items .= '<span class="bskicons"><a class="addone" href="' . SITE_PATH . 'add-one-to-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/" title="Add 1 of these items to your cart"><img src="' . SITE_PATH . 'images/cart-add.gif" title="Add 1 of these items to your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="remone" href="' . SITE_PATH . 'remove-one-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/" title="Remove 1 of these items from your cart"><img src="' . SITE_PATH . 'images/cart-remove.gif" alt="Remove 1 of these items from your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="delone" href="' . SITE_PATH . 'delete-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/" title="Remove this item completely"><img src="'. SITE_PATH . 'images/cart-delete.gif" alt="Remove this item completely" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a></span><span class="description-full">' . $res2['description'] . '</span>';

                                        if ($menucreate)
                                        {
                                            //get extras here and list as part of an item
                                            $iarray['extras'] = $this->getItemExtras($res2['id'], $res['id']);
                                        }
                                        array_push($itemlist, $iarray);
                                    }
                                    else
                                    {

                                        $iarray = array('id' => $res2['id'], 'menu_id' => $res['menu_id'], 'category' => $res['id'], 'name' => $res2['name'], 'ptype' => 'M', 'cost' => array(), 'description' => $res2['description'], 'extras' => array());
                                        //multiple price item, needs some extra stuff going on here
                                        $DBA3 = new DatabaseInterface();
                                        $fields3 = array(0 => 'description', 1 => 'cost');
                                        $idfields3 = array(0 => 'menu_item_id');
                                        $idvals3 = array(0 => $res2['id']);
                                        $orderby = 'id';

                                        $rs3 = $DBA3->selectQuery(DBITEMMULTIPLEPRICETABLE, $fields3, $idfields3, $idvals3, $orderby);

                                        if (!isset($rs3[2]))
                                        {
                                            $pcounter = 0;
                                            while ($res3 = mysql_fetch_array($rs3[0]))
                                            {
                                                unset($qty_bought);
                                                
                                                if (isset($_SESSION['basket_id']))
                                                {
                                                    $DBA4 = new DatabaseInterface();
                                                    $fields4 = array(0 => 'number_of_items');
                                                    $idfields4 = array(0 => 'basket_id', 1 => 'item_id', 2 => 'multiprice_description');
                                                    $idvals4 = array(0 => $_SESSION['basket_id'], 1 => $res['id'], 2 => str_replace(' ', '_',$res2['description']));

                                                    $rs4 = $DBA4->selectQuery(DBBASKETITEMSTABLE, $fields4, $idfields4, $idvals4);
                                                    unset($extracss);
                                                    unset($qty_bought);
                                                    if (!isset($rs4[2]))
                                                    {
                                                        while ($res4 = mysql_fetch_array($rs4[0]))
                                                        {
                                                            $qty_bought += $res4['number_of_items'];
                                                            $extracss = ' selectedline';
                                                        }
                                                    }
                                                    unset($DBA4, $fields4, $idfields4, $idvals4, $rs4);
                                                }

                                                $costs = array('description' => $res3['description'], 'cost' => $res3['cost']);
                                                array_push($iarray['cost'], $costs);
                                                unset($costs);

                                                if ($pcounter >= 1)
                                                {
                                                    $name = "";
                                                }
                                                else
                                                {
                                                    $name = $res2['name'] . ' - ';
                                                }
                                                
                                                $items2 .= '<span class="mpholder' . $extracss . '">';
                                                $items2 .= '<span class="menuname">' . $name . '<span class="mpdescription">' . $res3['description'] . '</span></span><span class="cost">' . $res3['cost'] . '</span>';

                                                if (isset($qty_bought))
                                                {
                                                    $items2 .= '<span class="qtyform ' . str_replace(' ','_',$res3['description']) . '">' . $qty_bought . '</span>';
                                                }
                                                else
                                                {
                                                    $items2 .= '<form action="' . SITE_PATH . 'add-to-cart/' . $this->StoreID . '/' . $this->ID . '/" id="' . str_replace(' ','_',$res3['description']) . $counter . '" name="' . str_replace(' ','_',$res3['description']) . $counter . '" method="post" ><input class="smqty" type="text" name="qty" value="" /><input type="submit" name="sub" value="ADD" class="pointer misub addone" /><input type="hidden" name="itemid" value="' . $res2['id'] . '" /><input type="hidden" name="catid" value="' . $res['id'] . '" /><input type="hidden" name="locationid" value="' . $this->LocationID . '" /><input type="hidden" name="mpdescription" value="' . str_replace(' ','_',$res2['description']) . '" /></form>';
                                                }
                                                //$html .= '</span>';
                                                $items2 .= '<span class="bskicons"><a class="addone" href="' . SITE_PATH . 'add-one-to-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/' . str_replace(' ','_',$res3['description']) . '/" title="Add 1 of these items to your cart"><img src="' . SITE_PATH . 'images/cart-add.gif" title="Add 1 of these items to your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="remone" href="' . SITE_PATH . 'remove-one-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/' . str_replace(' ','_',$res3['description']) . '/" title="Remove 1 of these items from your cart"><img src="' . SITE_PATH . 'images/cart-remove.gif" alt="Remove 1 of these items from your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="delone" href="' . SITE_PATH . 'delete-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/' . str_replace(' ','_',$res3['description']) . '/" title="Remove this item completely"><img src="'. SITE_PATH . 'images/cart-delete.gif" alt="Remove this item completely" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a></span>';     
                                                
                                                $items2 .= '</span>';
                                                $pcounter += 1;                       
                                            }
                                            $items .= '<li class="menuitem' . $extracss . ' mit' . $res2['id'] . ' multiprice">' . $items2;
                                            unset($extracss, $items2);
                                        }
                                        $items .= '<span class="description-full">' . $res2['description'] . '</span>';
                                        //<span class="extras">Extras here?</span>';
                                        if ($menucreate)
                                        {
                                            //get extras here and list as part of an item
                                            $iarray['extras'] = $this->getItemExtras($res2['id'], $res['id']);
                                        }
                                        array_push($itemlist, $iarray);
                                    }
                                    $counter += 1;
                                }
                                $items .= '</ul>';
                            }
                            else
                            {
                                $items .= 'No Items';
                            }
                            $items .= '</div>';
                        }
                        
                        if (isset($catlist))
                        {    
                            $tabcontent2 .= '<div class="subtabhld"><ul class="cattabs">' . $catlist . '</ul>' . $items . '</div>';
                        }
                        unset($items, $catlist);
                    }
                    else
                    {
                        $tabcontent2 = 'No Items';
                    }
                    unset($DBA, $res, $rs, $fields, $idfields, $idvals, $orderby);

                    $tabcontent .= $tabcontent2 . '</div>';
                    unset($tabcontent2);
                }
                $fincontent = '<div id="maintabhold"><ul id="maintabs">' . $maintabstr . '</ul>' . $tabcontent . '</div>';
            }
            return ($fincontent);
            /*if ($menucreate !== true)
            {
                $finstr = '<div id="menuinner">';
                $finstr .= '<a name="top"></a>';
                $finstr .= '<ul>';
                $finstr .= $lis;
                $finstr .= '</ul>';
                $finstr .= $html;
                $finstr .= '</div>';
                $finstr .= '</div>';

                return ($finstr);
            }
            else
            {
                return ($itemlist);
            }*/
        }

        function getMenuItems($menucreate=false)
        {
            $old_category = '';
            $itemlist = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description', 2 => 'ptype', 3 => 'cost', 4 => 'category', 5 => 'id');
            $idfields = array(0 => 'menu_id');
            $idvals = array(0 => $this->ID);
            $orderby = 'category, id';

            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals, $orderby);

            $counter = 0;
            unset($html, $lis);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if ($old_category != $res['category'])
                    {
                        $DBA2 = new DatabaseInterface();
                        $fields2 = array(0 => 'name');
                        $idfields2 = array(0 => 'id', 1 => 'menu_id');
                        $idvals2 = array(0 => $res['category'], 1 => $this->ID);

                        $rs2 = $DBA2->selectQuery(DBOLMENUCATEGORYTABLE, $fields2, $idfields2, $idvals2);
                        if (!isset($rs2[2]))
                        {
                            while ($res2 = mysql_fetch_array($rs2[0]))
                            {
                                $catname = stripslashes($res2['name']);
                                $catid = $res['category'];
                            }
                        }
                        unset($fields2, $idfields2, $idvals2, $rs2, $res2, $DBA2);
                       //make li record with $catname as id and text
                       $lis .= '<li><a href="#' . str_replace(",", "", str_replace("&", "and", str_replace("'", "", str_replace(' ','_',strtolower($catname))))) . '" title="View ' . ucwords($catname) . ' Items">' . ucwords($catname) . '</a></li>';
                       if ($old_category != '')
                       {
                            $html .= '</ul>';
                            $html .= '<a class="nav" href="#top">Back to top</a>';
                            $html .= '</div>';
                       }

                       $html .= '<div id="' . str_replace(",", "", str_replace("&", "and", str_replace("'", "", str_replace(' ', '_', strtolower($catname))))) . '">';
                       $html .= '<a class="nav" name="' . strtolower($catname) . '"></a>';
                       $html .= '<ul class="menu-items">';
                       $html .= '<li class="menuitem heads"><span class="menuname">Product Name</span><span class="cost">Cost(&pound;)</span><span class="form-column-header">Quantity</span></li>';
                    }

                    //start menu item

                    if ($res['ptype'] == 'S')
                    {
                        unset($qty_bought);

                        if (isset($_SESSION['basket_id']))
                        {
                            $DBA2 = new DatabaseInterface();
                            $fields2 = array(0 => 'number_of_items');
                            $idfields2 = array(0 => 'basket_id', 1 => 'item_id');
                            $idvals2 = array(0 => $_SESSION['basket_id'], 1 => $res['id']);

                            $rs2 = $DBA2->selectQuery(DBBASKETITEMSTABLE, $fields2, $idfields2, $idvals2);

                            unset($extracss);
                            if (!isset($rs2[2]))
                            {
                                while ($res2 = mysql_fetch_array($rs2[0]))
                                {
                                    $qty_bought += $res2['number_of_items'];
                                    $extracss = ' selectedline';
                                }
                            }
                            unset($DBA2, $fields2, $idfields2, $idvals2, $rs2);
                        }

                        $html .= '<li class="menuitem' . $extracss . ' mit' . $res['id'] . '">';
                        $html .= '<span class="menuname">' . $res['name'] . '</span><span class="cost">' . $res['cost'] . '</span>';
                        $iarray = array('id' => $res['id'], 'menu_id' => $res['menu_id'], 'category' => $res['category'], 'name' => $res['name'], 'ptype' => 'S', 'cost' => $res['cost'], 'description' => $res['description'], 'extras' => array());
                        //array_push($itemlist, $res);

                        if (isset($qty_bought))
                        {
                            $html .= '<span class="qtyform">' . $qty_bought . '</span>';
                        }
                        else
                        {
                            $html .= '<form action="' . SITE_PATH . 'add-to-cart/' . $this->StoreID . '/' . $this->ID . '/" id="' . $counter . '" name="' . $counter . '" method="post" ><input class="smqty" type="text" name="qty" value="" /><input type="submit" name="sub" value="ADD" class="pointer misub addone" /><input type="hidden" name="itemid" value="' . $res['id'] . '" /><input type="hidden" name="catid" value="' . $catid . '" /><input type="hidden" name="locationid" value="' . $this->LocationID . '" /></form>';
                        }
    //                    $html .= '</span>';
                        $html .= '<span class="bskicons"><a class="addone" href="' . SITE_PATH . 'add-one-to-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res['id'] . '/' . $catid . '/' . $this->LocationID . '/" title="Add 1 of these items to your cart"><img src="' . SITE_PATH . 'images/cart-add.gif" title="Add 1 of these items to your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="remone" href="' . SITE_PATH . 'remove-one-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res['id'] . '/' . $catid . '/' . $this->LocationID . '/" title="Remove 1 of these items from your cart"><img src="' . SITE_PATH . 'images/cart-remove.gif" alt="Remove 1 of these items from your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="delone" href="' . SITE_PATH . 'delete-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res['id'] . '/' . $catid . '/' . $this->LocationID . '/" title="Remove this item completely"><img src="'. SITE_PATH . 'images/cart-delete.gif" alt="Remove this item completely" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a></span><span class="description-full">' . $res['description'] . '</span>';

                        if ($menucreate)
                        {
                            //get extras here and list as part of an item
                            $iarray['extras'] = $this->getItemExtras($res['id'], $res['category']);
                        }
                        array_push($itemlist, $iarray);
                    }
                    else
                    {

                        $iarray = array('id' => $res['id'], 'menu_id' => $res['menu_id'], 'category' => $res['category'], 'name' => $res['name'], 'ptype' => 'M', 'cost' => array(), 'description' => $res['description'], 'extras' => array());
                        //multiple price item, needs some extra stuff going on here
                        $DBA2 = new DatabaseInterface();
                        $fields2 = array(0 => 'description', 1 => 'cost');
                        $idfields2 = array(0 => 'menu_item_id');
                        $idvals2 = array(0 => $res['id']);
                        $orderby = 'id';

                        $rs2 = $DBA2->selectQuery(DBITEMMULTIPLEPRICETABLE, $fields2, $idfields2, $idvals2, $orderby);

                        if (!isset($rs2[2]))
                        {
                            $pcounter = 0;
                            while ($res2 = mysql_fetch_array($rs2[0]))
                            {
                                unset($qty_bought);
                                
                                if (isset($_SESSION['basket_id']))
                                {
                                    $DBA3 = new DatabaseInterface();
                                    $fields3 = array(0 => 'number_of_items');
                                    $idfields3 = array(0 => 'basket_id', 1 => 'item_id', 2 => 'multiprice_description');
                                    $idvals3 = array(0 => $_SESSION['basket_id'], 1 => $res['id'], 2 => str_replace(' ', '_',$res2['description']));

                                    $rs3 = $DBA3->selectQuery(DBBASKETITEMSTABLE, $fields3, $idfields3, $idvals3);
                                    unset($extracss);
                                    unset($qty_bought);
                                    if (!isset($rs3[2]))
                                    {
                                        while ($res3 = mysql_fetch_array($rs3[0]))
                                        {
                                            $qty_bought += $res3['number_of_items'];
                                            $extracss = ' selectedline';
                                        }
                                    }
                                    unset($DBA3, $fields3, $idfields3, $idvals3, $rs3);
                                }

                                $costs = array('description' => $res2['description'], 'cost' => $res2['cost']);
                                array_push($iarray['cost'], $costs);
                                unset($costs);

                                if ($pcounter >= 1)
                                {
                                    $name = "";
                                }
                                else
                                {
                                    $name = $res['name'] . ' - ';
                                }
                                
                                $html2 .= '<span class="mpholder' . $extracss . '">';
                                $html2 .= '<span class="menuname">' . $name . '<span class="mpdescription">' . $res2['description'] . '</span></span><span class="cost">' . $res2['cost'] . '</span>';

                                if (isset($qty_bought))
                                {
                                    $html2 .= '<span class="qtyform ' . str_replace(' ','_',$res2['description']) . '">' . $qty_bought . '</span>';
                                }
                                else
                                {
                                    $html2 .= '<form action="' . SITE_PATH . 'add-to-cart/' . $this->StoreID . '/' . $this->ID . '/" id="' . str_replace(' ','_',$res2['description']) . $counter . '" name="' . str_replace(' ','_',$res2['description']) . $counter . '" method="post" ><input class="smqty" type="text" name="qty" value="" /><input type="submit" name="sub" value="ADD" class="pointer misub addone" /><input type="hidden" name="itemid" value="' . $res['id'] . '" /><input type="hidden" name="catid" value="' . $catid . '" /><input type="hidden" name="locationid" value="' . $this->LocationID . '" /><input type="hidden" name="mpdescription" value="' . str_replace(' ','_',$res2['description']) . '" /></form>';
                                }
            //                    $html .= '</span>';
                                $html2 .= '<span class="bskicons"><a class="addone" href="' . SITE_PATH . 'add-one-to-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res['id'] . '/' . $catid . '/' . $this->LocationID . '/' . str_replace(' ','_',$res2['description']) . '/" title="Add 1 of these items to your cart"><img src="' . SITE_PATH . 'images/cart-add.gif" title="Add 1 of these items to your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="remone" href="' . SITE_PATH . 'remove-one-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res['id'] . '/' . $catid . '/' . $this->LocationID . '/' . str_replace(' ','_',$res2['description']) . '/" title="Remove 1 of these items from your cart"><img src="' . SITE_PATH . 'images/cart-remove.gif" alt="Remove 1 of these items from your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="delone" href="' . SITE_PATH . 'delete-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res['id'] . '/' . $catid . '/' . $this->LocationID . '/' . str_replace(' ','_',$res2['description']) . '/" title="Remove this item completely"><img src="'. SITE_PATH . 'images/cart-delete.gif" alt="Remove this item completely" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a></span>';     
                                
                                $html2 .= '</span>';
                                $pcounter += 1;                       
                            }
                            $html .= '<li class="menuitem' . $extracss . ' mit' . $res['id'] . ' multiprice">' . $html2;
                            unset($extracss, $html2);
                        }
                        $html .= '<span class="description-full">' . $res['description'] . '</span>';
                        //<span class="extras">Extras here?</span>';
                        if ($menucreate)
                        {
                            //get extras here and list as part of an item
                            $iarray['extras'] = $this->getItemExtras($res['id'], $res['category']);
                        }
                        array_push($itemlist, $iarray);
                    }
//                    $html .= '<span class="hld">';                    
                    $html .= '</li>';
                    $counter += 1;
                    $old_category = $res['category'];
                }
            }
            if ($menucreate !== true)
            {
                $finstr = '<div id="menuinner">';
                $finstr .= '<a name="top"></a>';
                $finstr .= '<ul>';
                $finstr .= $lis;
                $finstr .= '</ul>';
                $finstr .= $html;
                $finstr .= '</div>';
                $finstr .= '</div>';

                return ($finstr);
            }
            else
            {
                return ($itemlist);
            }
        }

        function getCategories($itemcount=null)
        {
            $cats = array();
            $fields = array(0 => 'id', 1 => 'name', 2 => 'description', 3 => 'main_tab');
            $idfields = array(0 => 'menu_id');
            $idvals = array(0 => $this->ID);
            $orderby = 'main_tab, id asc';

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBOLMENUCATEGORYTABLE, $fields, $idfields, $idvals, $orderby);
            if ((!isset($rs[2])) && ($rs[1] >=1 ))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $safe = str_replace('&','and',str_replace(' ','-',strtolower($res['name'])));
                    $link = "#".$safe;
                    if (isset($itemcount))
                    {
                        $icount = $this->getNumberofCategoryItems($res['id']);
                        $temp = array('id' => $res['id'], 'name' => ucwords(stripslashes($res['name'])), 'itemcount' => $icount, 'description' => $res['description'], 'main_tab' => $res['main_tab'], 'safe-name' => $safe, 'navlink' => $link);
                    }
                    else
                    {
                        $temp = array('id' => $res['id'], 'name' => ucwords(stripslashes($res['name'])), 'description' => $res['description'], 'main_tab' => $res['main_tab'], 'safe-name' => $safe, 'navlink' => $link);
                    }
                    array_push($cats, $temp);
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            return($cats);
        }

        function getCategorybyID($id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $id);
            $rs = $DBA->selectQuery(DBOLMENUCATEGORYTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $name = stripslashes($res['name']);
                }
            }
            else
            {
                $name = 'fail';
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);
            return ($name);
        }

        function getNumberofCategoryItems($id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'count(id) as totitems');
            $idfields = array(0 => 'menu_id', 1 => 'category');
            $idvals = array(0 => $this->ID, 1 => $id);
            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $icount = $res['totitems'];
                }
            }
            else
            {
                $icount = 0;
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);
            return ($icount);
        }

        function getNumberofMasterCategoryItems($id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'count(id) as totitems');
            $idfields = array(0 => 'category');
            $idvals = array(0 => $id);
            $rs = $DBA->selectQuery(DBOLMASTERMENUITEMSTABLE, $fields, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $icount = $res['totitems'];
                }
            }
            else
            {
                $icount = 0;
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);
            return ($icount);
        }

        function getItem($itemid, $category)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description', 2 => 'ptype', 3 => 'cost', 4 => 'vegetarian', 5 => 'vegan', 6 => 'spice_rating', 7 => 'master_menu_id', 8 => 'id', 9 => 'mmi', 10 => 'choice_type', 11 => 'choice_mandatory', 14 => 'noofchoices');
            $idfields = array(0 => 'id', 1 => 'menu_id', 2 => 'category');
            $idvals = array(0 => $itemid, 1 => $this->ID, 2 => $category);

            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $item = $res;

                    if ($res['ptype'] == 'M')
                    {
                        $item['cost'] =array();

                        $DBA2 = new DatabaseInterface();
                        $fields2 = array(0 => 'description', 1 => 'cost');
                        $idfields2 = array(0 => 'menu_item_id');
                        $idvals2 = array(0 => $itemid);
                        $orderby = 'id';

                        $rs2 = $DBA2->selectQuery(DBITEMMULTIPLEPRICETABLE, $fields2, $idfields2, $idvals2, $orderby);

                        if (!isset($rs2[2]))
                        {
                            while ($res2 = mysql_fetch_array($rs2[0]))
                            {
                                $costs = array('description' => $res2['description'], 'cost' => $res2['cost']);
                                array_push($item['cost'], $costs);
                                unset($costs);
                            }
                        }
                    }
                }

                unset($DBA, $rs, $res, $fields, $idfields, $idvals);
                return $item;
            }
            else
            {
                return false;
            }
        }

        function getItemCategory($itemid) {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'category');
            $idfields = array(0 => 'id', 1 => 'menu_id');
            $idvals = array(0 => $itemid, 1 => $this->ID);

            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

            if((!isset($rs[2])) && ($rs[1] != 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $cat = $res['category'];
                }
            }

            unset($DBA, $rs, $res, $fields, $idfields, $idvals);
            return $cat;
        }

        function getItemExtras($iid, $cat=null,$mpdescrip=null)
        {
            $extras = array();
            $DBA3 = new DatabaseInterface();
            $fields3 = array(0 => 'id', 1  => 'name', 2 => 'cost', 3 => 'master_extra_category', 4 => 'master_extra_id');
            $idfields3 = array(0 => 'item_id', 1 => 'store_id', 2 => 'location_id', 3 => 'menu_id');
            $idvals3 = array(0 => $iid, 1 => $this->StoreID, 2 => $this->LocationID, 3 => $this->ID);

            if (isset($mpdescrip))
            {
                array_push($idfields3,'mpdescription');
                array_push($idvals3, $mpdescrip);
            }

            $rs3 = $DBA3->selectQuery(DBMENUITEMEXTRASTABLE, $fields3, $idfields3, $idvals3);
            if ((!isset($rs3[2])) && ($rs3[1] >= 1))
            {
                while ($res3 = mysql_fetch_array($rs3[0]))
                {
                    array_push($extras, $res3);
                }
            }
            unset($DBA3, $rs3, $res3, $fields3, $idfields3, $idvals3);
            return $extras;
        }

        function getItemswithExtras($gettype = null)
        {
            $items = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'distinct(item_id) as item_id');
            $idfields = array(0 => 'store_id', 1 => 'location_id', 2 => 'menu_id');
            $idvals = array(0 => $this->StoreID, 1 => $this->LocationID, 2 => $this->ID);

            $rs = $DBA->selectQuery(DBMENUITEMEXTRASTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if (!isset($gettype))
                    {
                        array_push($items, $res);
                    }
                    else
                    {
                        switch($gettype) {
                            case "name":
                                $name = $this->getItemName($res['item_id']);
                                $temp = array('itemid' => $res['item_id'], 'name' => $name);
                                array_push($items, $temp);
                                break;
                        }
                    }
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            return $items;
        }

        function getItemName($iid)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name');
            $idfields = array(0 => 'id', 1 => 'menu_id');
            $idvals = array(0 => $iid, 1 => $this->ID);
            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);
        
            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $retname = $res['name'];
                }
            }

            unset($DBA, $fields, $idfields, $idvals, $rs, $res);

            return $retname;
        }

        function getItemExtra($iid, $catid, $extid)
        {
            $DBA3 = new DatabaseInterface();
            $fields3 = array(0 => '*');
            $idfields3 = array(0 => 'item_id', 1 => 'menu_id', 2 => 'master_extra_category', 3 => 'master_extra_id', 4 => 'store_id', 5 => 'location_id');
            $idvals3 = array(0 => $iid, 1 => $this->ID, 2 => $catid, 3 => $extid, 4 => $this->StoreID, 5 => $this->LocationID);
            $rs3 = $DBA3->selectQuery(DBMENUITEMEXTRASTABLE, $fields3, $idfields3, $idvals3);
            if ((!isset($rs3[2])) && ($rs3[1] >= 1))
            {
                while ($res3 = mysql_fetch_array($rs3[0]))
                {
                    $retname = $res3;
                }
            }

            unset($DBA3, $fields3, $idfields3, $idvals3, $rs3, $res3);

            return $retname;
        }

        function getExtraInformation($iid, $extid)
        {
            $DBA3 = new DatabaseInterface();
            $fields3 = array(0 => '*');
            $idfields3 = array(0 => 'item_id', 1 => 'menu_id', 2 => 'id', 3 => 'store_id', 4 => 'location_id');
            $idvals3 = array(0 => $iid, 1 => $this->ID, 2 => $extid, 3 => $this->StoreID, 4 => $this->LocationID);
            $rs3 = $DBA3->selectQuery(DBMENUITEMEXTRASTABLE, $fields3, $idfields3, $idvals3);
            
            if ((!isset($rs3[2])) && ($rs3[1] >= 1))
            {
                while ($res3 = mysql_fetch_array($rs3[0]))
                {
                    $retname = $res3;
                }
            }

            unset($DBA3, $fields3, $idfields3, $idvals3, $rs3, $res3);

            return $retname;
        }

        function getItemChoiceType($iid)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'choice_type');
            $idfields = array(0 => 'menu_id', 1 => 'id');
            $idvals = array(0 => $this->ID, 1 => $iid);

            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    $choice_type = $res['choice_type'];
                }
            }

            unset($fields, $idfields, $idvals, $rs, $res, $DBA);

            return($choice_type);
        }

        function getChoiceInformation($iid, $chid)
        {
            $DBA3 = new DatabaseInterface();
            $fields3 = array(0 => '*');
            $idfields3 = array(0 => 'item_id', 1 => 'menu_id', 2 => 'id', 3 => 'store_id', 4 => 'location_id');
            $idvals3 = array(0 => $iid, 1 => $this->ID, 2 => $chid, 3 => $this->StoreID, 4 => $this->LocationID);

            $rs3 = $DBA3->selectQuery(DBMENUITEMCHOICETABLE, $fields3, $idfields3, $idvals3);

            if ((!isset($rs3[2])) && ($rs3[1] >= 1))
            {
                while ($res3 = mysql_fetch_array($rs3[0]))
                {
                    $retname = $res3;
                }
            }
            unset($DBA3, $fields3, $idfields3, $idvals3, $rs3, $res3);

            return $retname;
        }

        function getItemChoices($item_id,$multipdesc=null)
        {
            $temp = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'cost', 2 => 'id');
            $idfields = array(0 => 'store_id', 1 => 'location_id', 2 => 'menu_id', 3 => 'item_id');
            $idvals = array(0 => $this->StoreID, 1 => $this->LocationID, 2 => $this->ID, 3 => $item_id);
            
            if (isset($multipdesc))
            {
                array_push($idfields,'mpdescription');
                array_push($idvals, $multipdesc);
            }

            $rs = $DBA->selectQuery(DBMENUITEMCHOICETABLE, $fields, $idfields, $idvals);
            
            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }

            unset($DBA, $fields, $idfields, $idvals, $rs, $res);   

            return($temp);
        }

        function getItemsbyCat($id)
        {
            $temp = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'name');
            $idfields = array(0 => 'menu_id', 1 => 'category');
            $idvals = array(0 => $this->ID, 1 => $id);
            $order = 'id';
            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals, $order);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($temp, $res);
                }
            }

            unset($DBA, $fields, $idfields, $idvals, $rs, $res);   

            return($temp);
        }

        function getMealTypebyID($id) {
            switch ($id)
            {
                case 0:
                    $ing = 'chicken';
                    break;
                case 1: 
                    $ing = 'pork';
                    break;
                case 2:
                    $ing = 'beef';
                    break;
                case 3:
                    $ing = 'seafood';
                    break;
            }
            return ($ing);
        }

        function getItemsbyMealType($id)
        {
            $temp = array();
            switch ($id)
            {
                case 0:
                    $ing = 'chicken';
                    break;
                case 1: 
                    $ing = 'pork';
                    break;
                case 2:
                    $ing = 'beef';
                    break;
                case 3:
                    $ing = 'seafood';
                    break;
            }

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'name', 2 => 'mmi');
            $idfields = array(0 => 'menu_id');
            $idvals = array(0 => $this->ID);
            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $pos1 = strpos($res['mmi'], $ing);
                    if ($pos1 !== false)
                    {
                        array_push($temp, $res);
                    }
                }
            }
            unset($rs, $res, $fields, $idfields, $idvals, $DBA);

            return($temp);
        }

        function getMainCatbyID($id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $id);

            $rs = $DBA->selectQuery(DBMAINTABSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    $name = $res['name'];
                }
            }
            
            unset($DBA,$rs,$res,$fields,$idfields,$idvals);

            return($name);
        }

        function getItemsbyMainCat($id)
        {
            $temp = array();
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $idfields = array(0 => 'main_tab', 1 => 'menu_id');
            $idvals = array(0 => $id, 1 => $this->ID);
            $order = 'id';
            $rs = $DBA->selectQuery(DBOLMENUCATEGORYTABLE, $fields, $idfields, $idvals, $order);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    //the return is a category id, grab all ids and names of 
                    $temp = array_merge($temp, $this->getItemsbyCat($res['id']));
                }
            }

            unset($rs, $res, $fields, $idfields, $idvals, $DBA);
            return($temp);
        }

        function checkMenuStatus() {
            if ($this->Status != 1)
            {
                if ($this->Timed == 1)
                {
                    //do all the timed checking
                }
            }
            //otherwise we aren't checking for anythign else and status stays as it is
        }

        function getMultiprice($itemid,$itemname,$desc='') {

                //multiple price item, needs some extra stuff going on here
            $DBA3 = new DatabaseInterface();
            $fields3 = array(0 => 'description', 1 => 'cost');
            $idfields3 = array(0 => 'menu_item_id');
            $idvals3 = array(0 => $itemid);
            $orderby = 'id';

            $rs3 = $DBA3->selectQuery(DBITEMMULTIPLEPRICETABLE, $fields3, $idfields3, $idvals3, $orderby);
            if (!isset($rs3[2]))
            {
                $pcounter = 0;
                while ($res3 = mysql_fetch_array($rs3[0]))
                {
                    if ($pcounter == 0)
                    {
                        if ($desc != '')
                        {
                            $itemname .= ' - <small>' . $desc . '</small>';
                        }
                        $retstr .= '<div class="item-row row-fluid name-row">';
                        $retstr .= '<div class="span12 item-name">' . $itemname . '</div>';
                        $retstr .= '</div>';
                    }
                    $retstr .= '<div class="item-row row-fluid mp-row">';
                    $retstr .= '<div class="span9 item-desc">' . $res3['description'] . '</div>';
                    $retstr .= '<div class="span1 item-cost">' . $res3['cost'] . '</div>';
                    $retstr .= '<div class="span1 item-add button"><button type="submit" data-loading-text="..." class="btn btn-success btn-small" formaction="' . SITE_PATH . 'add-to-basket/' . $this->ID . '/' . $itemid . '/' . str_replace(' ','_',$res3['description']) . '/">ADD</button></div>';
                    $retstr .= '<div class="span1 hidden-phone">&nbsp;</div>';
                    $retstr .= '<div class="clearfix"></div>';
                    $retstr .= '</div>';
                    $pcounter += 1;
                    //$items2 .= '';
                    //$items2 .= '<span class="bskicons"><a class="addone" href="' . SITE_PATH . 'add-one-to-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/' . str_replace(' ','_',$res3['description']) . '/" title="Add 1 of these items to your cart"><img src="' . SITE_PATH . 'images/cart-add.gif" title="Add 1 of these items to your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="remone" href="' . SITE_PATH . 'remove-one-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/' . str_replace(' ','_',$res3['description']) . '/" title="Remove 1 of these items from your cart"><img src="' . SITE_PATH . 'images/cart-remove.gif" alt="Remove 1 of these items from your cart" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a><a class="delone" href="' . SITE_PATH . 'delete-from-cart/' . $this->StoreID . '/' . $this->ID . '/' . $res2['id'] . '/' . $res['id'] . '/' . $this->LocationID . '/' . str_replace(' ','_',$res3['description']) . '/" title="Remove this item completely"><img src="'. SITE_PATH . 'images/cart-delete.gif" alt="Remove this item completely" /><div class="weee"><img src="' . SITE_PATH . 'images/menuitem.png" alt="working" /></div></a></span>';     
                    
                }   
            }
            else
            {
                $retstr = '';
            }
            return($retstr);
        }
    }
?>
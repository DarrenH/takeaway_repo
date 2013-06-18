<?php
	    //********************************************************************//
    //   Application : Kooshtie Framework Layout                          //
    //   Filename : Class_Page.inc                           //
    //                                                                    //
    //   Author : Darren Hiles                                            //
    //   ©Kooshtie 2008 All Rights Reserved                               //
    //********************************************************************//

    class Page {
		var $ID,
		$Name,
		$Description,
		$Templatefile,
		$Templatelocation,
        $Dispmm,
        $Menupos,
        $Securitylevel,
		$Tags,
        $SEOTags,
		$Inputlist,
        $PNumber,
        $Extra,
        $After,
        $ExtraAfter,
        $Final,
        $FinalPlus,
        $Pop;

		function Page()
		{

		}

		function setPageVar($varname,$varvalue)
        {
            $this->$varname=$varvalue;
        }

		function getPage($ID)
		{
            $DBA = new DatabaseInterface();

			$fields = array(0 => 'name', 1 => 'description', 2 => 'templatefile', 3 => 'poptemplatefile', 4 => 'dispmm', 5 => 'seclevel');
			$idfields = array(0 => 'id');
			$idvals = array(0 => $ID);

			$rs = $DBA->selectQuery(DBPAGETABLE, $fields, $idfields, $idvals);

			if (!isset($rs[2]))
			{
				while ($res = mysql_fetch_array($rs[0]))
				{
					$this->setPageVar('ID', $ID);
					$this->setPageVar('Name', $res['name']);
					$this->setPageVar('Description', $res['description']);
                    if ($this->Pop === false)
                    {
					   $this->setPageVar('Templatefile', $res['templatefile']);
                    }
                    else
                    {
                        $this->setPageVar('Templatefile', $res['poptemplatefile']);
                    }
                    $this->setPageVar('Dispmm', $res['dispmm']);
                    $this->checkMenuPos();
                    $this->setPageVar('Securitylevel', $res['seclevel']);
				}
			}
		}

        function getTemplatebyName($name)
        {
            $this->setPageVar('Name', $name);

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'description', 2 => 'templatefile', 3 => 'dispmm', 4 => 'seclevel');
			$idfields = array(0 => 'name');
			$idvals = array(0 => $name);
            if ($this instanceof SectionPage)
            {
                $tablename = DBSECTIONSTABLE;
            }
            else
            {
                $tablename = DBPAGETABLE;
            }

			$rs = $DBA->selectQuery($tablename, $fields, $idfields, $idvals);

			if (!isset($rs[2]))
			{
				while ($res = mysql_fetch_array($rs[0]))
				{
					$this->setPageVar('ID', $res['id']);
					$this->setPageVar('Name', $name);
					$this->setPageVar('Description', $res['description']);
					$this->setPageVar('Templatefile', $res['templatefile']);
                    $this->setPageVar('Dispmm', $res['dispmm']);
                    $this->checkMenuPos();
                    $this->setPageVar('Securitylevel', $res['seclevel']);
				}
			}
        }

        function getPopTemplatebyName($name)
        {
            $this->setPageVar('Name', $name);

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'description', 2 => 'poptemplatefile', 3 => 'dispmm', 4 => 'seclevel');
            $idfields = array(0 => 'name');
            $idvals = array(0 => $name);
            if ($this instanceof SectionPage)
            {
                $tablename = DBSECTIONSTABLE;
            }
            else
            {
                $tablename = DBPAGETABLE;
            }

            $rs = $DBA->selectQuery($tablename, $fields, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setPageVar('ID', $res['id']);
                    $this->setPageVar('Name', $name);
                    $this->setPageVar('Description', $res['description']);
                    $this->setPageVar('Templatefile', $res['poptemplatefile']);
                    $this->setPageVar('Dispmm', $res['dispmm']);
                    $this->checkMenuPos();
                    $this->setPageVar('Securitylevel', $res['seclevel']);
                }
            }
        }

        function checkMenuPos()
        {
            if ($this->Dispmm == 1)
            {
                $DBA = new DatabaseInterface();
                $fields = array(0 => 'mmorder');
                $idfields = array(0 => 'id', 1 => 'name', 2 => 'type');
                $idvals = array(0 => $this->ID, 1 => $this->Name, 2 => 'p');

                $rs = $DBA->selectQuery(DBMENUTABLE, $fields, $idfields, $idvals);
                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        $this->setPageVar('Menupos', $res['mmorder']);
                    }
                }
                unset($res);
                unset($rs);
                unset($fields);
                unset($idfields);
                unset($idvals);
                unset($DBA);
            }
            else
            {
                $this->setPageVar('Menupos','');
            }
        }

		function getTemplate()
		{
			//using the app.kfc file we can determine the templates folder
			if (isset($this->Templatefile))
			{
				$this->setPageVar('Templatelocation', SERVER_ROOT.TPLFOLDER.$this->Templatefile);
			}
		}
		
		function getTemplateTags()
		{
			$tpl = fopen($this->Templatelocation, 'r');

			while (!feof($tpl))
			{
				//grab a line of text from the file
				$searchstr = fgets($tpl);

				//does it contain a tag?
				if (strpos($searchstr, '<tag'))
				{
					$searchstr = trim($searchstr);

					//we have found a tag now we need to use it to get the name/description/type of the item
					$tag = explode(' ', $searchstr);
					//$tag now contains somthing like
					//$tag[0] = '<tag'
					//$tag[1] = 'name="title"'
					//$tag[2] = 'description="Text which is displayed at the top of the browser window"'
					//$tag[3] = 'type="inputbox'
					//$tag[4] = ' />';

					$inputlist['name'] = $this->formTags($tag[1]); //name
					$inputlist['description'] = $this->formTags($tag[2]); //description
					$inputlist['type'] = $this->formTags($tag[3]); //type

                    if (($inputlist['name'] == 'keywords') || ($inputlist['name'] == 'description'))
                    {
                        if (isset($this->SEOTags[0]))
                        {
                            array_push($this->SEOTags, $inputlist);
                        }
                        else
                        {
                            $this->SEOTags[0] = $inputlist;
                        }
                    }
                    else
                    {
    					if (isset($this->Tags[0]))
	    				{
		    				array_push($this->Tags, $inputlist);
			    		}
				    	else
					    {
						    $this->Tags[0] = $inputlist;
    					}
                    }

					unset($this->Inputlist);
				}
			}

			fclose($tpl);
		}

        function getTemplatensTags()
		{
			$tpl = fopen($this->Templatelocation, 'r');
			while (!feof($tpl))
			{
				//grab a line of text from the file
				$searchstr = fgets($tpl);
                
				//does it contain a tag?
                $res = strpos($searchstr, '<tag');
				if ($res !== false)
				{
					$searchstr = trim($searchstr);

					//we have found a tag now we need to use it to get the name/description/type of the item
					$tag = explode(' ', $searchstr);
					//$tag now contains somthing like
					//$tag[0] = '<tag'
					//$tag[1] = 'name="title"'
					//$tag[2] = 'description="Text which is displayed at the top of the browser window"'
					//$tag[3] = 'type="inputbox'
					//$tag[4] = ' />';

					$inputlist['name'] = $this->formTags($tag[1]); //name
					$inputlist['description'] = $this->formTags($tag[2]); //description
					$inputlist['type'] = $this->formTags($tag[3]); //type

      				if (isset($this->Tags[0]))
       				{
        				array_push($this->Tags, $inputlist);
    	    		}
    		    	else
    			    {
    				    $this->Tags[0] = $inputlist;
      				}

					unset($this->Inputlist);
				}
			}

			fclose($tpl);
		}

		function formTags($line)
		{
			$pos = strpos($line, '"');
			$pos += 1;
			$pos2 = strrpos($line, '"');
			$pos2 -=1;
			$last = substr($line, $pos, $pos2-$pos1);
			return(substr($last, 0, -1));
		}

		function formInput($inputarray)
		{
			$labelstr = '<label for="' . $inputarray['name'] . '">' . str_replace('_', ' ', $inputarray['description']) . '</label>';

			//search db with $this->ID and $inputarray['name'] to get the value of the box
			$DBA = new DatabaseInterface();

			$fields = array(0 => 'inputvalue');
			$idfields = array(0 => 'pageid', 1 => 'inputname');
			$idvals = array(0 => $this->ID, 1 => $inputarray['name']);

			$rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals);

			if (!isset($rs[2]))
			{
				while ($res = mysql_fetch_array($rs[0]))
				{
					$value = html_entity_decode($res['inputvalue'], ENT_QUOTES, 'ISO-8859-1');
				}
			}

			//now make the input
            switch($inputarray['type'])
			{
                case 'inputbox':
					//<input type="text">
					$retstr .= $labelstr . '<input type="text" name="' . $inputarray['name'] . '" value="' . $value . '" />';
					break;
				case 'textarea':
					//<textarea>
					$retstr .= $labelstr . '<textarea name="' . $inputarray['name'] . '">' . $value . '</textarea>';
					break;
				case 'content':
					//fck
					$retstr = 'fck';
					break;
				case 'check':
					//<input type="checkbox">
					$retstr .= $labelstr . '<input type="checkbox" name="' . $inputarray['name'] . '" value="checked" class="check" ' . ($value=="checked" ? 'checked="checked" ' : '') . '/>';
					break;
                case 'filebox':
                    $retstr .= '<fieldset class="file"><span class="legend">File Upload</span>' . $labelstr;
                    if (isset($value))
                    {
                        $DBA = new DatabaseInterface();

                        $fields = array(0 => 'filename');
                        $idfields = array(0 => 'id');
                        $idvals = array(0 => $value);

                        $rs = $DBA->selectQuery(DBFILETABLE, $fields, $idfields, $idvals);
                        if ((!isset($rs[2])) && (!$rs[1]==0))
                        {
                            while ($res = mysql_fetch_array($rs[0]))
                            {
                                $retstr .= '<span class="filename">Current filename: ' . $res['filename'] . '<span class="delfile"><a href="' . SITE_PATH . ADMIN_FOLDER . 'delete_file/" title="Delete this file"><img src="' . SITE_PATH . ADMIN_FOLDER . 'images/delete-page.png" alt="Click here to delete this page" /><span>Delete this file</span></a></span></span>';
                                $retstr .= '<p>To Change this file please first delete the old version.</p>';
                                $retstr .= '<input type="hidden" class="hidden" name="' . $inputarray['name'] . '" value="' . $value . '" />';
                            }
                        }

                        $retstr = $labelstr . $retstr;

                        unset($fields);
                        unset($idfields);
                        unset($idvals);
                        unset($DBA);
                    }
                    else
                    {
                        $retstr .= '<input type="file" name="' . $inputarray['name'] . '" id="' . $inputarray['name'] . '" class="nobg" />';
                    }
                    $retstr .= '</fieldset>';
                    break;
				case 'radio':
					//<input type="radio">
					$reststr .= $labelstr . '<input type="radio" name="' . $inputarray['name'] . '" value="' . $value . '" />';
					break;
				case 'drop':
					//<select>
                    $sDBA = new DatabaseInterface();
                    $sfields = array(0 => 'id', 1 => 'name');

                    $sDBA = new DatabaseInterface();
                    $srs = $sDBA->selectQuery($inputarray['table'], $sfields);
                    if(!isset($srs[2]))
                    {
                        $retstr .= $labelstr . '<select name="' . $inputarray['name'] . '" id="' . $inputarray['name'] . '">';
                        while($sres = mysql_fetch_array($srs[0]))
                        {
                            $retstr .= '<option value="' . $sres['id'] . '"';
                            if ($sres['name'] == $inputarray['value'])
                            {
                                $retstr .= ' selected="selected"';
                            }
                            $retstr .= ' >' . $sres['name'] . '</option>';
                        }
                        $retstr .= '</select>';
                    }
                    unset($sfields);
                    unset($srs);
                    unset($sDBA);
					break;
				default:
					//<input type="text">
					$retstr .= $labelstr . '<input type="text" name="' . $inputarray['name'] . '" value="' . $res['value'] . '" />';
					break;
			}

			return $retstr;
		}

        function replaceTags($buffer)
        {
            if (count($this->Tags) >= 1)
            {
                foreach ($this->Tags as $Tag)
                {
                    if ($Tag['type'] != 'widget')
                    {
                         $DBA = new DatabaseInterface();

                         $fields = array(0 => 'inputvalue');

                         if ($this instanceof Page)
                         {
                            $idfields = array(0 => 'inputname', 1 => 'pageid');
                            $idvals = array(0 => $Tag['name'], 1 => $this->ID);
                         }
                         else
                         {
                            $idfields = array(0 => 'inputname', 1 => 'section_id');
                            $idvals = array(0 => $Tag['name'], 1 => $this->SectionID);
                         }
                         $rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals);
                         if (!isset($rs[2]))
                         {
                            while ($res = mysql_fetch_array($rs[0]))
                            {
                                $replacestr = '<tag name="' . $Tag['name'] . '" description="' . $Tag['description'] . '" type="' . $Tag['type'] . '" />';
                                $replacewithstr = html_entity_decode($res['inputvalue'], ENT_QUOTES, 'ISO-8859-1');
                                $buffer = str_replace($replacestr, $replacewithstr, $buffer);
                            }
                         }

                         unset($rs);
                         unset($idvals);
                         unset($idfields);
                         unset($fields);
                         unset($DBA);
                    }
                    else
                    {
                         $Widget = new Widget(null, $Tag['name']);
                         $widgetbuffer = $Widget->getVal('Buffer');
                         $replacestr = '<tag name="' . $Tag['name'] . '" description="' . $Tag['description'] . '" type="' . $Tag['type'] . '" />';
                         $replacewithstr = $widgetbuffer;
                         $buffer = str_replace($replacestr, $replacewithstr, $buffer);
                         unset($Widget);
                    }
                }
            }
            return($buffer);
        }

        function makePageDetailsForm()
        {
            if ($this->ID != 0)
            {
                $type = "Page";
                $id = $this->ID;
            }
            else
            {
                $type = "Section";
                $id = $this->SectionID;
            }

            $form = '<form enctype="multipart/form-data" id="detailform" name="detailform" action="' . SITE_PATH . ADMIN_FOLDER . 'save-details/" method="post">';
            $form .= '<fieldset>';
            $form .= '<span class="legend">' . $type . ' ' . ucwords($this->Name) . ' Details</span>';
            $form .= '<label for="name">Name</label><input type="text" id="name" name="name" value="' . $this->Name . '" />';
            $form .= '<label for="description">Description</label><input type="text" id="desc" name="desc" value="' . $this->Description . '" />';
            $form .= '<label for="tfile">Template File</label><input type="text" name="tfile" id="tfile" value="' . $this->Templatefile . '" />';
            $form .= '<span class="checkholder">';
            $form .= '<label for="dispmm" class="large">Display on Main menu?</label><input type="checkbox" class="check" id="dispmm" name="dispmm" ' . ($this->Dispmm == 1 ? 'checked="checked"' : '') . '/>';
            $form .= '</span>';
            $form .= '<label for="mmorder" class="clear">Order on Main menu</label><input type="text" id="mmorder" name="mmorder" value="' . $this->Menupos . '" />';
            $form .= '<label for="seclev" class="largelabel">Please choose the security level of the page. Note anything above guest will need specific user accounts to be set up</label>';
            unset($options);
            $seclevels = getSecurityLevels();

            foreach ($seclevels as $sec)
            {
                if ($sec['id'] == $this->Securitylevel)
                {
                    $extra = ' selected="selected"';
                }

                $options .= '<option value="' . $sec['id'] . '"' . $extra . '>' . $sec['name'] . '</option>';

                unset($extra);
            }

            if (isset($options))
            {
                $form .= '<select name="seclev" id="seclev">' . $options . '</select>';
            }

            if ($type == 'Section')
            {
                $form .= '<label for="itemtype">Enter the name of the item type eg (News Item for news)</label><input type="text" id="itemtype" name="itemtype" value="' . $this->SectionItem . '" />';
            }
            $form .= '<label for="id" class="offscreen hidden">ID</label><input type="hidden" class="offscreen hidden" id="id" name="id" value="' . $id . '" />';
            $form .= '<label for="type" class="offscreen hidden">Type</label><input type="hidden" class="offscreen hidden" id="type" name="type" value="' . $type . '" />';
            $form .= '</fieldset>';
            $form .= '<input id="detailsave" type="submit" name="detailsave" value="save" class="submit" />';
            $form .= '<input id="detailcancel" type="submit" name="detailcancel" value="cancel" class="submit" />';
            $form .= '</form>';

            return ($form);
        }

        function makePageSEOForm()
        {
            if ($this->ID != 0)
            {
                $type = "Page";
                $id = $this->ID;
            }
            else
            {
                $type = "Section";
                $id = $this->SectionID;
            }
            $form = '<form enctype="multipart/form-data" id="seoform" name="seoform" action="' . SITE_PATH . ADMIN_FOLDER . 'save-seo/" method="post">';
            $form .= '<fieldset>';
            $form .= '<span class="legend">' . $type . ' ' . ucwords($this->Name) . ' SEO Fields</span>';
            foreach ($this->SEOTags as $stags)
            {
                $ret = $this->formInput($stags, $itemid);

                if ($ret == 'fck')
                {
                    $form .= '<span class="fck">';
                    $form .= '<label for="' . $tags['name'] . '">' . str_replace('_', ' ',$tags['description']) . '</label>';
                    $sBasePath = SITE_PATH . ADMIN_FOLDER . 'fckeditor/' ;

                    $fckname = $tags['name'];

                    $oFCKeditor = new FCKeditor($fckname) ;
                    $oFCKeditor->BasePath = $sBasePath ;
                    $oFCKeditor->Height = '400';
                    if ($User->AdminCPView == 0)
                    {
                        $oFCKeditor->ToolbarSet = 'Simple';
                    }
                    else
                    {
                        $oFCKeditor->ToolbarSet = 'Advanced';
                    }
                    $sDBA = new DatabaseInterface();
                    $fields = array(0 => 'inputvalue');
                    $idfields = array(0 => 'section_id', 1 => 'item_id', 2 => 'inputname');
                    $idvals = array(0 => $Section->SectionID, 1 => $itemid, 2 => $tags['name']);

                    $srs = $sDBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals);

                    if (!isset($srs[2]))
                    {
                    	while ($sres = mysql_fetch_array($srs[0]))
                    	{
                    		$value = html_entity_decode($sres['inputvalue'], ENT_QUOTES, 'ISO-8859-1');
                    	}
                    }

                    $oFCKeditor->Value = $value;
                    $oFCKeditor->Create();
                    unset($value);
                    $form .= '</span>';
                }
                else
                {
                    $form .= $ret;
                }

                unset($ret);
            }


            $form .= '<label for="id" class="offscreen hidden">ID</label><input type="hidden" class="offscreen hidden" id="id" name="id" value="' . $id . '" />';
            $form .= '<label for="type" class="offscreen hidden">Type</label><input type="hidden" class="offscreen hidden" id="type" name="type" value="' . $type . '" />';
            $form .= '</fieldset>';
            $form .= '<input id="seosave" type="submit" name="seosave" value="save" class="submit" />';
            $form .= '<input id="seocancel" type="submit" name="seocancel" value="cancel" class="submit" />';
            $form .= '</form>';
            return($form);
        }

        function deleteMenu()
        {
            $sql = "delete from " . DBMENUTABLE . " where id = " . $this->ID . " and name = '" . str_replace(' ', '-', $this->Name) . "' and type = 'p'";

            $DBA = new DatabaseInterface();

            $rs = $DBA->runQuery($sql);

  		    unset($sql);
      		unset($DBA);

            if (isset($rs[2]))
            {
                return(1);
            }
            else
            {
                return(0);
            }
        }

        function saveMenu()
        {
            unset($menuerror);
            $fields = array(0 => 'id', 1 => 'name', 2 => 'type', 3 => 'mmorder');
            $fieldvals = array(0 => $this->ID, 1 => str_replace(' ', '_',$this->Name), 2 => 'p', 3 => $this->Menupos);
            $DBA = new DatabaseInterface();
            $rs = $DBA->insertQuery(DBMENUTABLE, $fields, $fieldvals);
            if (isset($rs2[2]))
            {
               $menuerror = 1;
            }
            unset($rs);
      		unset($fields);
         	unset($fieldvals);
      		unset($DBA);

            if (isset($menuerror))
            {
                return (2);
            }
            else
            {
                return(0);
            }
        }

        function savePage()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description', 2 => 'templatefile', 3 => 'seclevel', 4 => 'dispmm');
  		    $fieldvals = array(0 => str_replace(' ', '_',$this->Name), 1 => $this->Description, 2 => $this->Templatefile, 3 => $this->Securitylevel, 4 => $this->Dispmm);
            $idfield = 'id';
            $idval = $this->ID;
            $rs = $DBA->updateQuery(DBPAGETABLE, $fields, $fieldvals, $idfield, $idval);

      		if ($rs == 1)
      		{
                if ($this->Dispmm == '1')
                {
                    //save a menu record
                    $error = $this->saveMenu();
                }
      		}
      		else
      		{
                $error = 1;
      		}

      		unset($rs,$fields,$fieldvals,$DBA);

            return ($error);
        }

        function deletePageContent()
        {
            $DBA = new DatabaseInterface();
            $idfield = 'pageid';
            $idval = $this->ID;
            $rs = $DBA->deleteRecord(DBPAGECONTENTTABLE, $idfield, $idval);
            if (isset($rs[2]))
            {
                return(1);
            }
            else
            {
                return(0);
            }
        }

        function deletePageRecord()
        {
            $DBA = new DatabaseInterface();
            $idfield = 'id';
            $idval = $this->ID;
            $rs = $DBA->deleteRecord(DBPAGETABLE, $idfield, $idval);
            if (isset($rs[2]))
            {
                return(1);
            }
            else
            {
                return(0);
            }
        }

        function deletePage()
        {
            unset($error);
            $res1 = $this->deleteMenu();
            $res2 = $this->deletePageContent();
            $res3 = $this->deletePageRecord();
            if ($res1 == 1)
            {
                error_push('Error deleting menu record.');
                $error = 1;
            }
            if ($res2 == 1)
            {
                error_push('Error deleting page content');
                $error = 1;
            }
            if($res3 == 1)
            {
                error_push('Error deleting page record');
                $error = 1;
            }

            return($error);
        }
	}
?>

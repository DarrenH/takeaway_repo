<?php
    class APP
    {
        private $PageName;
        private $PageType;
        private $PageExtra;
        private $PageNumber;
        private $AfterVars;
        private $SecondAfter;
        private $FinalArg;
        private $TypeID;
        private $ServerPath;
        private $SitePath;
        private $TemplateFile;
        private $TemplateServerLocation;
        private $TemplateSiteLocation;
        private $Tags;
        private $Pop;
        private $BufferText;
        private $Portal;
        private $PortalCode;

        function __construct($passvars=null)
        {
            $this->checkPageType($passvars);
            if ($this->getVal('PageType') != '404')
            {
                if ((isset($passvars['v2'])) && (isset($passvars['v3'])))
                {
                    //we have and extra field probs a category
                    $this->setVal('PageExtra', $passvars['v2']);
                    $this->setVal('PageNumber', $passvars['v3']);
                }
                elseif ((isset($passvars['v2'])) && (!isset($passvars['v3'])))
                {
                    $this->setVal('PageNumber', $passvars['v2']);
                }

                if (isset($passvars['v4']))
                {
                    $this->setVal('AfterVars', $passvars['v4']);
                }

                if (isset($passvars['v5']))
                {
                    $this->setVal('SecondAfter', $passvars['v5']);
                }

                if (isset($passvars['v6']))
                {
                    $this->setVal('FinalArg', $passvars['v6']);
                }
                if (isset($passvars['v7']))
                {
                    $this->setVal('FinalArgPlus', $passvars['v7']);
                }
                if (isset($passvars['v8']))
                {
                    $this->setVal('ExtraPlus', $passvars['v8']);
                }
            }
            $this->getPage();
            $this->preparePage();
        }

        private function setVal($varname, $varvalue)
        {
            $this->$varname=$varvalue;
        }

        function getVal($varname)
        {
            return $this->$varname;
        }

        private function checkfor($tablename, $id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $idfields = array(0 => 'name');
            $idvals = array(0 => $id);
            $rs = $DBA->selectQuery($tablename, $fields, $idfields, $idvals);
        
            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                $result = 1;
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setVal('TypeID', $res['id']);
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals, $tablename);

            return $result;
        }

        private function checkExisting($page)
        {
            unset($return);

            $this->setVal('PageName', $page['v1']);

            $ALLOWABLE_PAGE_EXTENSIONS = array(0 => 'html', 1 => 'php', 2 => 'xml', 3 => 'pdf');
            unset($found);

            $pfilename = array();
            $pfilename2 = array();

            foreach($ALLOWABLE_PAGE_EXTENSIONS as $ext)
            {
                $serverpath = SERVER_ROOT . $page['v1'] . '.' . $ext;

                if (file_exists($serverpath))
                {
                    $return = 1;
                    $this->setVal('TemplateFile', $page['v1'] . '.' . $ext);
                    break;
                }
            }

            return $return;
        }

        private function checkPageType($vartocheck)
        {
            unset($this->PageType);
            $this->setVal('PageName', $vartocheck['v1']);
            //first check the store names table

            if ($this->checkfor(DBSTORETABLE, $vartocheck['v1']) == 1)
            {
                $this->setVal('PageType', 'store');
            }

            //next check user
            if ((!isset($this->PageType)) && ($this->checkfor(DBUSERTABLE, $vartocheck['v1']) == 1))
            {
                $this->setVal('PageType', 'user');
            }

            //then check database created page
            if ((!isset($this->PageType)) && ($this->checkfor(DBPAGETABLE, $vartocheck['v1']) == 1))
            {
                $this->setVal('PageType', 'dynamic-single');
            }

            if ((!isset($this->PageType)) && ($this->checkfor(DBSECTIONSTABLE, $vartocheck['v1']) == 1))
            {
                $this->setVal('PageType', 'dynamic-iterate');
            }

            //then check existing
            if (!isset($this->PageType))
            {
                if (($this->PageName=='newsletter') || ($this->PageName=='tracker'))
                {
                    //page is a newsletter or we are requesting the tracker image.
                    if ($this->PageName == 'newsletter')
                    {
                        $this->setVal('PageType', 'newsletter');
                    }
                    else
                    {
                        $this->setVal('PageType', 'tracker');
                    }
                }
                elseif ($this->checkExisting($vartocheck) == 1)
                {
                    //we have a page
                    $this->setVal('PageType', 'physical');
                }
                else
                {
                    //404!
                    $this->setVal('PageType', '404');
                }
            }

            if ((isset($vartocheck['v2'])) && ($vartocheck['v2'] == 'pop'))
            {
                $this->setVal('Pop', true);
            }
            else
            {
                $this->setVal('Pop', false);
            }
        }

        private function getPage()
        {
            switch ($this->getVal('PageType')) {
                case "404":
                    $this->setVal('TemplateFile', ERROR_PAGE_TEMPLATE);
                    break;
                case "physical":
                    //already taken care of as we had the value we needed earlier
                    break;
                case "store":
                    $this->setVal('TemplateFile', STORE_TEMPLATE_FILE);
                    break;
                case "user":
                    $this->setVal('TemplateFile', USER_TEMPLATE_FILE);
                    break;
                case "dynamic-single":
                    $Page = new Page();
                    if ($this->Pop === false)
                    {
                        $Page->getTemplatebyName($this->getVal('PageName'));
                    }
                    else
                    {
                        $Page->getPopTemplatebyName($this->getVal('PageName'));
                    }
                    $this->setVal('TemplateFile', $Page->Templatefile);
                    unset($Page);
                    break;
                case "dynamic-iterate":
                    $Section = new SectionPage();
                    $Section->getTemplatebyName($this->getVal('PageName'));
                    $this->setVal('TemplateFile', $Section->Templatefile);
                    unset($Section);
                    break;
                default:
                    //we should never get here but just in case...
                    $this->setVal('TemplateFile', ERROR_PAGE_TEMPLATE);
                    break;
            }
            $this->setVal('TemplateServerLocation', SERVER_ROOT . ($this->getVal('PageType') != "physical" ? TPLFOLDER : '') . $this->getVal('TemplateFile'));
            $this->setVal('TemplateSiteLocation', SITE_PATH . ($this->getVal('PageType') != "physical" ? TPLFOLDER : '') . $this->getVal('TemplateFile'));
        }

        private function preparePage()
        {
            //now we do all kinds of things
            switch ($this->getVal('PageType')) {
                case "physical":
                    $Page = new Page();
                    $Page->setPageVar('PNumber', $this->PageNumber);
                    $Page->setPagevar('Extra', $this->PageExtra);
                    $Page->setPageVar('After', $this->AfterVars);
                    $Page->setPageVar('ExtraAfter', $this->SecondAfter);
                    $Page->setPageVar('Final', $this->FinalArg);
                    $Page->setPageVar('FinalPlus', $this->FinalArgPlus);
                    $Page->setPageVar('Arg', $this->ExtraPlus);
                    ob_start();
                    include($this->TemplateServerLocation);
                    $buffer = ob_get_contents();
                    ob_end_clean();
                    $this->setVal('BufferText', $buffer);
                    $this->checkForWidgets();
                    break;
                case "dynamic-single":
                    $Page = new Page();
                    if ($this->Pop === false)
                    {
                        $Page->getTemplatebyName($this->getVal('PageName'));
                    }
                    else
                    {
                        $Page->getPopTemplatebyName($this->getVal('PageName'));
                    }
                    $Page->getTemplate();
                    $Page->getTemplatensTags();
                    $Page->setPageVar('PNumber', $this->PageNumber);
                    $Page->setPageVar('Extra', $this->PageExtra);
                    $Page->setPageVar('After', $this->AfterVars);
                    $Page->setPageVar('ExtraAfter', $this->SecondAfter);
                    $Page->setPageVar('Final', $this->FinalArg);
                    $Page->setPageVar('FinalPlus', $this->FinalArgPlus);
                    ob_start();
                    include($this->TemplateServerLocation);
                    $buffer = ob_get_contents();
                    ob_end_clean();
                    $this->setVal('BufferText', $buffer);
                    $this->setVal('BufferText', $Page->replaceTags($this->getVal('BufferText')));
                    break;
                case "dynamic-iterate":
                    $Section = new SectionPage();
                    $Section->getSectionPage(getSectionID($this->getVal('PageName')));
                    $Section->getTemplatebyName($this->getVal('PageName'));
                    $Section->getTemplate();
                    $Section->getsectionTemplatensTags();
                    if (!is_numeric($this->PageNumber))
                    {
                        $Section->setPageVar('MultiPage', $this->getVal('PageNumber'));
                        $this->setVal('PageNumber', '');
                    }
                    else
                    {
                        $Section->setPageVar('PageNumber', $this->getVal('PageNumber'));
                    }
                    ob_start();
                    include($this->TemplateServerLocation);
                    $buffer = ob_get_contents();
                    ob_end_clean();
                    $this->setVal('BufferText', $buffer);
                    $this->checkForWidgets();
                    //$this->setVal('BufferText', $Section->replaceTags($this->getVal('BufferText')));
                    $htmlpos = strpos($this->getVal('BufferText'), '</html>');
                    $totlen = strlen($this->getVal('BufferText'));
                    $chopfrom = $totlen - $htmlpos;
                    $chopfrom -= 7;
                    $buffer = substr($this->getVal('BufferText'), 0, -$chopfrom);
                    $this->setVal('BufferText', $buffer);
                    break;
                case "404":
                    ob_start();
                    include($this->TemplateServerLocation);
                    $buffer = ob_get_contents();
                    ob_end_clean();
                    $this->setVal('BufferText', $buffer);
                    $this->checkForWidgets();
            }
        }

        private function checkForWidgets()
        {
            $this->getTags();
            if (count($this->Tags) >= 1)
            {
                foreach ($this->Tags as $Tag)
                {
                    if ($Tag['type'] == 'widget')
                    {
                        $Widget = new Widget(null, $Tag['name']);
                        $widgetbuffer = $Widget->getVal('Buffer');
                        $replacestr = '<tag name="' . $Tag['name'] . '" description="' . $Tag['description'] . '" type="' . $Tag['type'] . '" />';
                        $replacewithstr = $widgetbuffer;
                        $endtext = str_replace($replacestr, $replacewithstr, $this->BufferText);
                        $this->setVal('BufferText', $endtext);
                    }
                }
            }
        }

        function getTags()
		{
			$tpl = fopen($this->TemplateServerLocation, 'r');
			while (!feof($tpl))
			{
				//grab a line of text from the file
				$searchstr = fgets($tpl, 4096);

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

        function showPage()
        {
            print_r($this->BufferText);
        }
    }


?>
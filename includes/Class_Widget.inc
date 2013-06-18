<?php
    class Widget {
        private $ID;
        private $Name;
        private $TemplateFile;
        private $TemplateLocation;
        private $Buffer;
        private $AssociatedSection;
        private $Tags;

        function __construct($id=nul, $name=null)
        {
            if ((isset($id)) || (isset($name)))
            {
                if (isset($id))
                {
                    $type = 'i';
                    $findval = $id;
                }
                else
                {
                    $type = 'n';
                    $findval = $name;
                }
                $this->getWidget($type, $findval);
            }
        }

        function setVal($varname, $varvalue)
        {
            $this->$varname = $varvalue;
        }

        function getVal($varname)
        {
            return $this->$varname;
        }

        private function getWidget($type, $findval)
        {
            $fields = array(0 => 'template_file', 1 => 'associated_section');
            switch ($type) {
                case "i":
                    array_push($fields, 'name');
                    $idfields = array(0 => 'id');
                    break;
                case "n":
                    array_push($fields, 'id');
                    $idfields = array(0 => 'name');
                    break;
            }
            $idvals = array(0 => $findval);
            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBWIDGETTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) || (!$rs[1] == 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    switch($type) {
                        case "i":
                            $this->setVal('ID', $findval);
                            $this->setVal('Name', $res['name']);
                            break;
                        case "n":
                            $this->setVal('Name',  $findval);
                            $this->setVal('ID', $res['id']);
                            break;
                    }
                    $this->setVal('TemplateFile', $res['template_file']);
                    $this->setVal('AssociatedSection', $res['associated_section']);
                    $this->setTemplateLocation();
                    $this->loadTemplateCode();
                }
            }
        }

        private function setTemplateLocation()
        {
            $this->setVal('TemplateLocation', SERVER_ROOT . TPLFOLDER . WIDGETFOLDER . $this->TemplateFile);
        }

        private function loadTemplateCode()
        {
            ob_start();
            include($this->TemplateLocation);
            $widgetcode = ob_get_contents();
            ob_end_clean();
            $this->setVal('Buffer', $widgetcode);
            unset($widgetcode);
        }
    }
?>
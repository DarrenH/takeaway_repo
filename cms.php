<?php
    session_cache_limiter('nocache');
    session_start();
    header('Cache-Control: no-cache, must-revalidate, post-check=3600, pre-check=3600');
    ini_set('zlib.output_compression', 4096);
    ini_set('zlib.output_compression_level', 5);
    ob_start("gz_handler");
    include('App.kfc');
    include(SERVER_ROOT . 'includes/functions.inc');
    include(SERVER_ROOT . 'includes/section_page_item_functions.inc');
    include(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    include(SERVER_ROOT . 'includes/Class_Page.inc');
    include(SERVER_ROOT . 'includes/Class_SectionPage.inc');
    include(SERVER_ROOT . 'includes/Class_User.inc');
    include(SERVER_ROOT . 'includes/Class_Basket.inc');
    //include(SERVER_ROOT . 'includes/Class_Newsletter.inc');
    include(SERVER_ROOT . 'includes/Class_Store.inc');
    /*include(SERVER_ROOT . 'includes/Class_CuisineType.inc');
    include(SERVER_ROOT . 'includes/Class_DeliveryArea.inc');*/
    include(SERVER_ROOT . 'includes/Class_Menu.inc');
    /*include(SERVER_ROOT . 'includes/Class_Category.inc');
    include(SERVER_ROOT . 'includes/Class_Item.inc');
    include(SERVER_ROOT . 'includes/Class_Cart.inc');*/
    include(SERVER_ROOT . 'includes/Class_Order.inc');
    include(SERVER_ROOT . 'includes/Class_Widget.inc');
    include(SERVER_ROOT . 'includes/Class_App.inc');

    $DBA = new DatabaseInterface();

    $App = new App($HTTP_GET_VARS);
    //first off check the first option passed by htaccess
    $App->showPage();
    unset($App);
    ob_end_flush();
?>
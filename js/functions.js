function preparePage() {
	/*locateUser();*/
	saveStore();
	checkDimensions();
	hideElements();
	prepareAddButtons();
	prepareViewButtons();
	prepareAccountButtons();
	prepareCheckboxes();
	prepareChoiceSelectButtons();
	prepareBasketButtons();
	prepareButtonLoading();
	prepareContactButton();
}

function hideElements() {
	var windowWidth=$(window).width();
	if (windowWidth<=480) {
		$('body').addClass('mobile');
		$('#catnavul').hide('slow');
		$('#catulbottom').show('fast');
		$('#showmenu').show();
		$('#sidebarnav h2').on('click', 'span', function() {
			if($('#catnavul').css('display') == 'block') {
				showel('#catulbottom');
				hideel('#catnavul');
			}
			else
			{
				hideel('#catulbottom');
				showel('#catnavul');
			}
		})
	}
}

function locateUser() {
	if (navigator.geolocation) {
    	navigator.geolocation.getCurrentPosition(locateSuccess, locateFail);
	}
}

function saveStore() {
	var test = jQuery.data(document.body, 'storeinfo');
	if (typeof test == 'undefined')
	{
	$.ajax({
		type: "GET",
		url: "../get_store_info.php",
		data: "json=true",
		dataType: "json",
		async: false,
		success: function(data) {
			jQuery.data(document.body, 'storeinfo', data);
		} 
	});
}
}

function checkDimensions() {
	var windowWidth=$(window).width();var headerHeight=$('#header').height();var basketHeight=$('#basket').height();var sideBarNavHeight=$('#catnav').height() - parseInt($('#sidebarnav').css('paddingLeft')) - parseInt($('#sidebarnav').css('paddingRight'));var totalSideBarHeight = sideBarNavHeight + basketHeight;
	if (windowWidth>999){var offset = parseInt($('.container-fluid').css('marginRight')) + parseInt($('.container-fluid').css('paddingRight'));$('#basket').hide().css('right', offset).fadeIn('slow');$('#messages').css('right', offset);}
	if ((totalSideBarHeight < $(window).height()) && (windowWidth>760)){var dataOffset = headerHeight - basketHeight;$('#sidebarnav').attr('data-spy','affix').attr('data-offset-top',dataOffset);var sideBarNavWidth=$('#catnav').width() - parseInt($('#sidebarnav').css('paddingLeft')) - parseInt($('#sidebarnav').css('paddingRight'));$('#sidebarnav').css('width', sideBarNavWidth);}
	else{$('.cathead').append('<div class="itemrow btt"><a href="#top" title="Back to top">Back to top</a></div>');}
}

function prepareButtonLoading() {
	$(document).on('click', '.btn', function() {
		var btn = $(this);
		btn.button('loading');
		setTimeout(function() {btn.button('reset')},1000);
	})
}

function checkForCandE(item) {
	$.ajax({
        type: "GET",
        url: "/check-item-addons.php",
        data: item[2],
        dataType: "json",
        async: false,
        success: function(data) {
        	if (data.htmlstr !== '')
        	{
	        	result = data.htmlstr;
        	}
        	else
        	{
        		result = "";
        	}
        },
        error: function(error) {
        	result = false;
        }
    });
	return result;
}

function addToBasket(item) {	
	var htmlstr;
	$.ajax({
        type: "GET",
        url: "/add-to-basket.php",
        data: item[2],
        dataType: "json",
        async: false,
        success: function(returned2){
    		
        },
        error: function () {
        
        }
    });

    return true;
}

function prepareAddButtons() {
	$('.item-add button').click(
		function(ev){
			ev.preventDefault();
			var bskt = $('#basket');
			bskt.children().hide();
			bskt.hide('fast');
			var btn = $(this);
			btn.button('loading');
			btn.parent('.item-row').css('backgroundColor', '#c2c2c2')
			setTimeout(function() {btn.button('reset')},1000);
			var itemarray = prepareItemInfo(btn.attr('formaction'));

			addToBasket(itemarray);
			alertMsg('Item added successfully','success');
			var resp = checkForCandE(itemarray);
			if (resp)
			{
				//modal add choices/extras and show
				$('#choicesextrasModal').html(resp).removeData("modal").modal({backdrop: 'static', keyboard: false}).modal('show');
			}
			var tots = getTotals();
			var bskdvspan = $('#basket div span');
			bskdvspan.html('').html(tots);
			bskt.show('fast');
			bskt.children().show();
			btn.parent('.item-row').css('backgroundColor', '');
		}
	);
}

function prepareItemInfo(itemurl) {
	var itemidarray = itemurl.split('/');
	var itemidarraycount = itemidarray.length;

	var temparray = new Array;
	temparray[0] = itemidarray[0] + "://" + itemidarray[2];
	temparray[1] = itemidarray[0] + "://" + itemidarray[2] + "/" + itemidarray[3];
	var datastring = "json=true&mid=" + itemidarray[4] + "&iid=" + itemidarray[5];
	temparray[3] = itemidarray[4];
	temparray[4] = itemidarray[5];
	if (itemidarraycount == 8)
	{
		//multiprice
		datastring = datastring + "&mpdesc=" + itemidarray[6];
		temparray[5] = itemidarray[6];
		temparray[6] = 'M';
	}
	else
	{
		temparray[5] = '';
		temparray[6] = 'S';
	}
	temparray[2] = datastring;
	return temparray;
}

function getTotals() {
	$.ajax({
        type: "GET",
        url: "/get-basket-totals.php",
        data: "json=true",
        dataType: "json",
        async: false,
        success: function(returned2){
        	htmlstr = "Items = " + returned2.totitems + " Total = &pound;" + parseFloat(returned2.totalcost).toFixed(2);
        },
        error: function () {
        	htmlstr = "";
        }
    });
    return htmlstr;
}

function prepareViewButtons() {
	$('#basket').on('click', 'a.btn', function(ev) {
		ev.preventDefault();
		var btn = $(this);
		btn.button('loading');
		setTimeout(function() {btn.button('reset')},1000);
		var target = $(this).attr('data-target');
		var url = $(this).attr('data-url') + "pop/";
		$(target).html('').html(getBasketHTML(url));
	});
}

function getBasketHTML(fileurl) {
	var htmlstr;
	$.ajax({
		type: "GET",
		url: fileurl,
		data: "json=true",
		dataType: "json",
		async: false,
		success: function (returned){
			htmlstr = returned.html;			
		}
	});

	return htmlstr;
}

function prepareAccountButtons() {
	$('#login').click(function (ev) {
		ev.preventDefault();
	});

	$('#register').click(function (ev){
		var mail = $('#regmail').val();
		var pass = $('#regpass').val();

		if (mail != '')
		{
			$('#rmail').val(mail);
		}
		if (pass != '')
		{
			$('#rpass1').val(pass);
		}
	});

	$('#regModal').on('click','#regsub',function() {
		var uname = $('#uname').val();
		var mail = $('#rmail').val();
		var pass = $('#rpass1').val();
		var pass2 = $('#rpass2').val();

		var dstr = '&uname=' + uname + '&mail=' + mail + '&pass1=' + pass + '&pass2' + pass2;

		$.ajax({
			type: "GET",
			url: '../login-proc.php',
			data: "json=true"+dstr,
			dataType: "json",
			async: false,
			success: function (returned){
				htmlstr = returned.html;			
				$('#regModal').html('').html(htmlstr);
			}
		});		
	});
}

function prepareCheckboxes() {
	$('label.checkbox').click(function (ev) {
		ev.preventDefault();
		var inpdom = $(this).children('input[type="checkbox"]');
		var check = inpdom.prop('checked');
		if (check == false)
		{
			inpdom.prop('checked',true).attr('checked','checked');
		}
		else
		{
			inpdom.prop('checked', false).removeAttr('checked');
		}
	});
	$('input[type="checkbox"]').click(function (ev) {
		ev.preventDefault();
		var it = $(this);
		var check = $(this).prop('checked');
		if(check == false)
		{
			$('this').prop('checked', true).attr('checked','checked');
		}
		else
		{
			$(this).prop('checked', false).removeAttr('checked');
		}
		return false;
	});
}

function prepareChoiceSelectButtons() {
	$('#choicesextrasModal').on('click', 'button[type="submit"]', function(ev) {
		ev.preventDefault();
		var selector;
		var ectype;
		var extras = $('input[name="extrasel"]').length;
		if (extras >= 1)
		{
			ectype = "E";
			selector = $('input[name="extrasel"]:checked');
		}
		else
		{
			ectype = "C";
			selector = $('select.choicesel');
			var tcount = selector.length;
			if (tcount == 0)
			{
				//no selector found so its radioboxes that are checked
				selector = $('input[name="choicesel"]:checked');
			}
		}
		//send to extras save;
		var menuid = $('#menuid').val();
		var storeid = $('#storeid').val();
		var locationid = $('#locationid').val();
		var itemid = $('#itemid').val();
		if ($('#mpdesc').length >= 1)
		{
			var mpdesc = $('#mpdesc').val();
		}
		var formdata = {"values":[
			{
				"storeid" : storeid,
				"locationid" : locationid,
				"menuid" : menuid,
				"itemid" : itemid,
				"mpdesc" : mpdesc
			}
		]}
		var counter = 0;
		var cid = [];
		selector.each(function () {
			cid.push($(this).val());
		});
		ectype == "E" ? saveExtras(formdata,cid) : saveChoices(formdata,cid);
		var msg = "saved successfully";
		ectype == "E" ? msg = "Extras " + msg : msg = "Choices " + msg;
		alertMsg(msg,'success');
	});
}

function saveExtras(fdata,cid) {
	//save to basket extras table
	var dtstr;
	var counter;
	var upto = fdata.values.length - 1;
	for (counter = 0;counter <= upto;counter++)
	{
		dtstr = "storeid=" + fdata.values[counter]['storeid'];
		dtstr += "&locationid=" + fdata.values[counter]['locationid'];
		dtstr += "&menuid=" + fdata.values[counter]['menuid'];
		dtstr += "&itemid=" + fdata.values[counter]['itemid'];
		if (fdata.values[counter]['mpdesc'] != '')
		{
			dtstr += "&mpdesc=" + fdata.values[counter]['mpdesc'];
		}
		dtstr += "&eid=" + cid;
	}
	$.ajax({
		type: "GET",
		url: "../save-item-extras.php",
		data: dtstr,
		dataType: "json",
		async: false,
		success: function (returned){
			$('#choicesextrasModal').modal('hide').html('');
			$('#viewModal').html('');
		}
	});

	return true;
}

function saveChoices(fdata,cid) {
	//save to basket choices table
	var dtstr;
	var counter;
	var upto = fdata.values.length - 1;
	for (counter = 0;counter <= upto;counter++)
	{
		dtstr = "storeid=" + fdata.values[counter]['storeid'];
		dtstr += "&locationid=" + fdata.values[counter]['locationid'];
		dtstr += "&menuid=" + fdata.values[counter]['menuid'];
		dtstr += "&itemid=" + fdata.values[counter]['itemid'];
		if (fdata.values[counter]['mpdesc'] != '')
		{
			dtstr += "&mpdesc=" + fdata.values[counter]['mpdesc'];
		}
		
		var upto = cid.length - 1;
		for (counter = 0;counter <= upto;counter++)
		{
			dtstr += "&cid" + counter + "=" + cid[counter];
		}
	}
	$.ajax({
		type: "GET",
		url: "../save-item-choices.php",
		data: dtstr,
		dataType: "json",
		async: false,
		success: function (returned){
			$('#choicesextrasModal').modal('hide').html('');
			$('#viewModal').html('');
		}
	});

	return true;
}

function prepareBasketButtons() {
	var mod = $('#viewModal');
	mod.on('click', 'button#clearbsk', function(ev) {
		$.ajax({
			type: "GET",
			url: '../clear-basket.php',
			data: "json=true",
			dataType: "json",
			async: false,
			success: function (returned){
				mod.html('').html(returned.html);
				$('#basket div').html('').html('<span>Items = 0 Total = &pound;0.00</span><a data-loading-text="...loading..." data-toggle="modal" data-target="#viewModal" data-url="../view-basket/" class="btn" title="basket">Basket</a><div class="clearfix"></div>');	
			}
		});
	});

	mod.on('click', 'button#conf, button#goback', function(ev) {
		var store = jQuery.data(document.body, 'storeinfo');
		var loc = store.store.SetLocationID;
		var delivers = store.store.Locations[loc][13];
		var stepnumber;
		//mod.children('.modal-body').html('').html('<h5>Please Wait</h5><div class="progress"><div class="bar bar-info" style="width:10%"></div><div class="bar bar-warning" style="width:20%; display:none;"></div><div class="bar bar-success" style="width:70% display:none;"></div></div>');
		switch (delivers) {
			case "Y":
				stepnumber = 1;
				break;
			case "N":
				stepnumber = 2;
				break;
		}
		dtstr = "step=" + stepnumber;
		$.ajax({
			type: "GET",
			url: '../next-step.php',
			data: dtstr,
			dataType: "json",
			async: false,
			/*beforeSend: function () {
				mod.find('.modal-body div.bar-warning').show();
			},*/
			success: function (returned3) {
				var cont = returned3.htmlstr;
				mod.html('').html(cont);				
			}
		});
		
		if (typeof(jQuery.data(document.body,'deliverydets'))!=='undefined') {
			var data = jQuery.data(document.body,'deliverydets');
			var house = data[0];
			$('#ohouse').val(data[0]);
			$('#ostreet').val(data[1]);
			$('#otown').val(data[2]);
			$('#ocounty').val(data[3]);
			$('#opcode').val(data[4]);
		}
	});

	mod.on('click', 'input[name="delval"]', function(ev) {
		$(this).attr('id') == 'delvalC' ? hideel('#deladdr') : showel('#deladdr');
	});

	mod.on('click', 'button#delprog', function(ev){
		$('span.help-inline').remove();
		$('.control-group').removeClass('error');
		var flag = true;
		//delivery info added...check info is correct and proceed...
		var tsel = $('input[name="delval"]:checked').val();
		if (typeof(tsel) === "undefined")
		{
			$('input[name="delval"]').parents('.control-group').addClass('error').append('<span class="help-inline">You must select either delivery or collection</span>');
			flag = false;
		}

		if (flag != false)
		{
			var datastr = '';
			if (tsel === 'C')
			{
				//no need to worry about delinfo
				jQuery.data(document.body, 'deliveryinfo', 'C');
			}
			else
			{
				//delivery info needs checked and saved...

				jQuery.data(document.body, 'deliveryinfo', 'D');
				
				var id = 'opcode';
				var tbval = $('#opcode').val();
				if(checkInput(id,tbval) !== true)
				{
					$('#opcode').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">Invalid Postcode</span>');
					flag = false;
				}

				if ($('#ohouse').val() == '')
				{
					$('#ohouse').parents('.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">Please enter a house number</span>');
				}

				if ($('#ostreet').val() == '')
				{
					$('#ostreet').parents('.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">Please enter a street name</span>');
				}

				if (flag != false)
				{
					var data = Array(5);
					data[0] = $('#ohouse').val();
					data[1] = $('#ostreet').val();
					data[2] = $('#otown').val();
					data[3] = encodeURIComponent($('#ocounty').val());
					data[4] = $('#opcode').val();
					jQuery.data(document.body, 'deliverydets', data);
					datastr = '&dtype=D&dhouse=' + data[0] + '&dstreet=' + data[1] + '&dtown=' + data[2] + '&dcounty=' + data[3] + '&dpcode=' + data[4];
				}
			}
		}

		if (datastr != '')
		{
			datastr = "step=2" + datastr;
		}
		else
		{
			datastr = "step=2";
		}

		if (flag != false) {
			$.ajax({
				type: "GET",
				url: '../next-step.php',
				data: datastr,
				dataType: "json",
				async: false,
				beforeSend: function () {
					mod.find('.modal-body div.bar-warning').show();
				},
				success: function (returned3) {
					var cont = returned3.htmlstr;
					mod.find('.modal-body div.bar-success').show();
					setTimeout(function() {mod.html('').html(cont);}, 1000);
				}
			});
		}
	});

	mod.on('click', 'button#finalconfirm', function (ev) {
		$.ajax({
			type: "GET",
			url: '../next-step.php',
			data: "step=3",
			dataType: "json",
			async: false,
			beforeSend: function () {
				mod.find('.modal-body div.bar-warning').show();
			},
			success: function (returned3) {
				var cont = returned3.htmlstr;
				mod.find('.modal-body div.bar-success').show();
				setTimeout(function() {mod.html('').html(cont);}, 1000);
			}
		});
	});

	mod.on('click', '#acccreate', function(ev) {
		var boxval = $('#acccreate').prop('checked');
		boxval === true ? showel('#passcontrol') : hideel('#passcontrol');
	});

	mod.on('click', 'button#contprog', function(ev) {
		$('span.help-inline').remove();
		$('.control-group').removeClass('error');
		var fail = false;
		//contact details added...
		var fname = $('#cfname').val();
		var sname = $('#csname').val();
		var email = $('#cemail').val();
		var telno = $('#ctel').val();
		var cracc = $('#acccreate').prop('checked');
		var pass = $('#pass').val();
		//is the create account box checked
		if (telno !== '')
		{
			if(jQuery.isNumeric(telno) === false)
			{
				fail = true;
				$('#ctel').parents('.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">Invalid Telephone Number</span>');
			}
		}
		//are any text vals empty?
		if ((fname == '') || (sname == '') || (email == '') || (telno == ''))
		{
			//fail
			fail = true;
			if (fname == '') {
				$('#cfname').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter a last name.</span>');
			}
			if (sname == '') {
				$('#csname').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter a first name.</span>');
			}
			if (email == '') {
				$('#cemail').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter an email address .</span>');
			}
			if (telno == '') {
				$('#ctel').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter a telephone number.</span>');
			}
		}

		if ((cracc === true) && (pass == ''))
		{
			fail = true;
			$('#pass').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter a password for your new account.</span>');
		}

		if (fail === false) {
			var str ='step=4';
			str += '&ufname=' + fname + '&usname=' + sname + '&uemail=' + email + '&utel=' + telno;
			if (cracc)
			{
				str += '&accreate=1&passval=' + pass;
			}
			else
			{
				str += '&acccreate=0';
			}

			$.ajax({
				type: "GET",
				url: '../next-step.php',
				data: str,
				dataType: "json",
				async: false,
				beforeSend: function () {
					mod.find('.modal-body div.bar-warning').show();
				},
				success: function (returned3) {
					var cont = returned3.htmlstr;
					mod.find('.modal-body div.bar-success').show();
					setTimeout(function() {mod.html('').html(cont);}, 1000);
				}
			});
		}
	});

	mod.on('click', 'button.bskbut', function(ev) {
		var url;

		var urlstr = $(this).attr('formaction');
		var itemidarray = urlstr.split('/');
		var datastr = '&basket_table_id=' + itemidarray[2] + '&type=' + checkClass($(this));
		
		$.ajax({
			type: "GET",
			url: '../basket-button-action.php',
			data: "json=true"+datastr,
			dataType: "json",
			async: false,
			success: function (returned){
				var bskt = $('#basket');
				bskt.children().hide();
				bskt.hide('fast');
				mod.html('').html(returned.html);
				$('#basket div span').html('').html(returned.totstr);
				var html2 = getTotals();
				var bskdvspan = $('#basket div span');
				bskdvspan.html('').html(html2);
				bskt.show('fast');
				bskt.children().show();
			}
		});
	});

	mod.on('click','button#pchoiceprog', function() {
		var ptype = $('#deliveryinfo input[name="pchoiceopt"]:checked').val();
		if (ptype === 'pp') {
			str = "step=6";
		}
		else
		{
			str = "step=5&type="+ptype;
		}
		$.ajax({
			type: "GET",
			url: '../next-step.php',
			data: "json=true&"+str,
			dataType: "json",
			async: false,
			success: function (returned){
				mod.html('').html(returned.htmlstr);
			}
		});
	});

	mod.on('click', 'button#finito', function() {
		$.ajax({
			type: "GET",
			url: '../next-step.php',
			data: "step=7",
			dataType: "json",
			async: false,
			success: function () {}
		});
		var bskdvspan = $('#basket div span');
		bskdvspan.html('').html('Items = 0 Total = &pound;0.00');
	});
}

function checkInput(selector, tval) {
	switch(selector) {
		case "opcode":
			var postcodeRegEx = /[A-Z]{1,2}[0-9]{1,2} ?[0-9][A-Z]{2}/i; 
    		return postcodeRegEx.test(tval);
    	default:
    		return true;
	}
}

function checkClass(obj) {
	var ret;
	var add = obj.hasClass('add');
	var rem = obj.hasClass('rem');
	var del = obj.hasClass('del');

	if (add === true)
	{
		ret = 'add';
	}
	else if (rem === true)
	{
		ret = 'rem';
	}
	else if (del === true)
	{
		ret = 'del';
	}

	return ret;
}

function alertMsg(msg, atype) {
	var aclass = 'alert-' + atype;
	$('#messages').addClass(aclass).children('span').html('').html(msg);
	$('#messages').show('fast');
	setTimeout(function() {$('#messages').removeClass(atype).hide('slow').children('span').html('')},2000);
}

function hideel(selector) {
	$(selector).hide();
}

function showel(selector) {
	$(selector).show();
}

function prepareContactButton() {
	var fail = false;
	$('span.help-inline').remove();
	$('.control-group').removeClass('error');
	var mod = $('#contModal');
	mod.on('click', 'button#send', function(ev) {
		ev.preventDefault();
		var name = $('#cont-name').val();
		var email = encodeURIComponent($('#cont-email').val());
		var message = encodeURIComponent($('#cont-mess').val());
		var fakeemail = encodeURIComponent($('#email').val());

		if (fakeemail != '') 
		{
			fail=true;
			mod.html('<div class="alert">Shame on you!</div>');
		}

		if ((email == '') || (message == ''))
		{
			fail = true;
			if (email == '') {
				$('#cont-email').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter a email address.</span>');
			} else {
				$('#cont-mess').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter a message.</span>');
			}
		}

		if (fakeemail != '')
		{
			fail=true;
			$('#cont-email').parents('div.control-group').addClass('error').end().parents('div.controls').append('<span class="help-inline">You must enter a email address.</span>');
		}

		if (fail === false)
		{
			var datastr;

			if(name!='') {
				datastr = "name=" + name + "&";
			}
			datastr += "email=" + email + "&message=" + message;
			$.ajax({
		        type: "GET",
				url: '../send-contact.php',
		        data: datastr,
		        dataType: "json",
		        async: false,
				success: function (returned){
					mod.html('').html(returned.htmlstr);
				}
			});
		}
	});
}

function locateSuccess(position) {
	var latit = position.coords.latitude;
	var longit = position.coords.longitude;
	geocoder = new google.maps.Geocoder();
	var latlng = new google.maps.LatLng(latit, longit);
    geocoder.geocode({'latLng': latlng}, function(results, status) {
    if (status == google.maps.GeocoderStatus.OK) {
        //ajax call to check_local(results[1].address_components[3].long_name);
        $.ajax({
            type: "GET",
            url: "http://www.cheaptakeawaymenus.com/check-user-locale-jax.php",
            data: "pcode=" + results[1].address_components[3].long_name,
            dataType: "json",
            async: false,
		        success: function (qresult) {
                if (qresult[0] == 'success') {
                    var msgtxt = "We have located your position and have determined the first two letters of your postcode are " + results[1].address_components[3].long_name + ". Is this correct?";
                }
            }
        });
      } else {
        //console.log("Geocoder failed due to: " + status);
		        }
		    });
		}

function locateFail(positionError) {
	$.ajax({
        type: "GET",
        url: "http://www.cheaptakeawaymenus.com/user-locale-deny-jax.php",
        data: "",
        dataType: "json",
        async: false,
        success: function (qresult) {             
        }
	});
}
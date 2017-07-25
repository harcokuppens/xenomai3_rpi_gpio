<?php

# dyndns server 

$verify_mac=true; # if set to false you can also use a hostname or dns name to do lookup
$verify_ip=true;  # if set to false you can store any string 
$verify_remote_ip=true; # if set to true, you can only use the site from machines within network with $network_prefix
$network_prefix="131.174";  # wan ip must match network prefix; no prefix, allow any ip to be set in this database
$lan_prefix="192.168";  # lan ip must match network prefix; no prefix, allow any ip to be set in this database

$datafile='/tmp/dnsdata.txt';
$allow_to_print_all_data = false; # if set true then after each request also all current registered entries are listed

# This php server script can be combined with /etc/profile.d/dyndns.sh script
# which is run on each login and updates mac-ip combination.
# For this dyndns server script one has to configure in the first two lines of the dyndns.sh script :


# note for lab pi's for the students we use 
#  - in client dyndns.sh script:
#      network_prefix="131.174"    => only allows  ip addresses within 131.174.*.* network
#      lan_prefix="192.168"        => allows local lan ip addresses
#      dyndns_url="http://www.cs.ru.nl/lab/dyndns/"
#  - in server php script
#      $network_prefix="131.174";
    
# note for my own pi's I use   -> so that they can be used also at other places and with hostnames
#  - in client dyndns.sh script:
#      network_prefix=".*"    => allows updating all ip addresses
#      lan_prefix="192.168"   => allows local lan ip addresses
#      dyndns_url="http://hidden/url"
#  - in server php script
#      $network_prefix="";  # no prefix, allow any ip to be set in this database
#      $verify_mac=false; # if set to false you can also use a hostname or dns name to do lookup

# bash shell helpers 
#
#    url_dyndns_server="http://www.cs.ru.nl/lab/dyndns/"
#    getipfrommac() { curl "$url_dyndns_server?mac=$1&cmdline=1" 2>/dev/null; }
#    setmac2ip() { curl "$url_dyndns_server?mac=$1&ip=$2&cmdline=1" 2>/dev/null;  }
#    getallmac2ipdata() { curl "$url_dyndns_server?data=1&cmdline=1" 2>/dev/null; }

#
# usage:
#   $ setmac2ip c8:1f:66:bb:5f:2c 131.174.30.57
#   $ getipfrommac c8:1f:66:bb:5f:2c
#   131.174.30.57
#
# thus:
#    run setmac2ip when booting machine after getting ip
#    then othermachines can use the mac address to get ip of that machine  





// helper functions
// --------------------------



// src: http://technologyordie.com/php-mac-address-validation
function format_mac($mac, $format='linux'){
 
    $mac = trim($mac);
    $len = strlen($mac);
    $mac = preg_replace("/[^a-fA-F0-9:]/",'',$mac);
    if ( $len != strlen($mac) ) 
        return false;  // removed illegal character
 
    $mac = preg_replace("/[^a-fA-F0-9]/",'',$mac);

    $mac = (str_split($mac,2));
    if(!(count($mac) == 6))
        return false;
 
    if($format == 'linux' || $format == ':'){
        return $mac[0]. ":" . $mac[1] . ":" . $mac[2]. ":" . $mac[3] . ":" . $mac[4]. ":" . $mac[5];
    }elseif($format == 'windows' || $format == '-'){
        return $mac[0]. "-" . $mac[1] . "-" . $mac[2]. "-" . $mac[3] . "-" . $mac[4]. "-" . $mac[5];
    }elseif($format == 'cisco'){
        return $mac[0] . $mac[1] . ":" . $mac[2] . $mac[3] . ":" . $mac[4] . $mac[5];
    }else{
        return $mac[0]. "$format" . $mac[1] . "$format" . $mac[2]. "$format" . $mac[3] . "$format" . $mac[4]. "$format" . $mac[5];
    }
}

function output_result($result_msg,$error_msg) {
    global $via_form,$allow_to_print_all_data,$data_cmd;
    if ( $via_form ) {
       echo "<html><br/><br/><br/><center>";         
       if ($error_msg) echo "<p style='color:#FF0000'>$error_msg</p>";
       $formdata = <<<HERE
       <h1> Lookup ip using mac address </h1>
       <form action="index.php" method="get">
         Mac address:  <input type="text" name="mac"><br>
         <br/>
         <input type="submit" value="Lookup IP">
       </form> 
HERE;
       echo "$formdata";        
    } else{
       if ($error_msg) echo "error:$error_msg"; // no stderr, so must prefix with "error:" to designate error
    }
    if ($result_msg ) echo "$result_msg";
    if ( $allow_to_print_all_data ) { 
        $data= get_data();
        if ( $via_form ) {
           echo "</br>"; 
           echo "</br><h2>All data:</h2>"; 
           var_dump($data);
        } else {     
           if ($data_cmd ) {     
              print_r($data);
           } 
        }        
    }
    if ( $via_form ) {
       echo "</center></html>"; 
    }   
    exit(0);
}

function errorexit($error_msg) {
    output_result("",$error_msg);
}

function get_data() {
    global $datafile;

    $oldstring = file_get_contents($datafile);
    $data= unserialize( $oldstring );
    
    return $data;
}

function save_data($data) {
    global $datafile;

    $newstring = serialize( $data ); 
    file_put_contents($datafile, $newstring, LOCK_EX);
    
    return;
}


// get and verify parameters
// --------------------------

$mac= $_GET["mac"];
$ip= $_GET["ip"];
$data_cmd=$_GET["data"];
$remote_ip=$_SERVER['REMOTE_ADDR'];


$via_form = true;
if ( $_GET["cmdline"] ) $via_form =false;

if ( $verify_remote_ip && substr( $remote_ip, 0, strlen($network_prefix)  ) != "$network_prefix" ) {
    $allow_to_print_all_data=0; // to be sure no data is leaked
    errorexit("Requests are only allowed from remote machines with ip within network with prefix $network_prefix");        
}

// basic verification ; usefull for cmdline to check parameter is given but not set 
if (array_key_exists("mac", $_GET) && !$mac) {
    errorexit("missing mac");
}

if (array_key_exists("ip", $_GET) && !$ip) {
    errorexit("missing ip");
}




if ( $verify_mac && $mac != "" ) {
    $mac=format_mac($mac,'linux'); // returns false if not valid mac
    if (  ! $mac   )  {
        errorexit("invalid mac");
    } 
}

if (  $verify_ip && $ip != "" ) {
    $ip = trim($ip);
    $ip=filter_var($ip, FILTER_VALIDATE_IP); //return false if not valid ip
    if ( !  $ip  )  {
        errorexit("invalid ip");
    } 
    
    if (  substr( $ip, 0, strlen($network_prefix)  ) != "$network_prefix" &&   substr( $ip, 0, strlen($lan_prefix)  ) != "$lan_prefix" ) {
        errorexit("invalid ip, must be within network prefix $network_prefix  or local network prefix $lan_prefix  ");
    } 
}  



// handle parameters
// -----------------


if ( !$via_form && $data_cmd ) { // cmd to output all data!! (only for cmdline, gui shows it always)
    if ( ! $allow_to_print_all_data ) errorexit("not allowed");
    output_result("");    
}

# no get nor set  
if ( !$mac ) {
  if ( $via_form ) { 
    output_result(""); //  in case of form just show empty form,
  } else {
    errorexit("missing mac"); //in case of cmdline just return error 
  }    
}

# get
if ( $mac && ! $ip  ) {
  

  $data= get_data();
  
  $value=$data["$mac"];
  $pieces = explode(";", $value);
  $ip=$pieces[0];
  $timestamp=$pieces[1];
  
  // different output result via form or cmdline
  if ( $via_form ) {
      if  ( $value ) {
         output_result("<br><br><h2> Result:   <br><br>  $ip  </br>at</br> $timestamp</h2>"); 
      } else {
         output_result("<br><br><h2> Result:   <br><br>  nothing found</h2>");
      }     
  } else {
      output_result($value); # so found ip can be directly used, where empty string means nothing found
  }
  
} 

# set
if ( $mac &&  $ip  ) {
  
  $timestamp=date("Y-m-d H:i:s");
  

  $data= get_data();
  $data["$mac"]="$ip;$timestamp";
  save_data( $data ); 
  
  // different output result via form or cmdline
  if ( $via_form ) {
      output_result("<br><br><h2> Result:   <br><br> set mac=$mac to ip=$ip</h2>"); 
  } else {
      output_result("$mac $ip");
  }
}





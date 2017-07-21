<?php

# dyndns server without ip restrictions

$verify_mac=true; # if set to false you can also use a hostname or dns name to do lookup
$verify_ip=true;  # if set to false you can store any string 
$network_prefix="";  # no prefix, allow any ip to be set in this database

$datafile='/tmp/dnsdata.txt';
$print_all_data = false; # if set true then after each request also all current registered entries are listed

# This php server script can be combined with /etc/profile.d/dyndns.sh script
# which is run on each login and updates mac-ip combination.
# For this dyndns server script one has to configure in the first two lines of the dyndns.sh script :


# note for lab pi's for the students we use 
#  - in client dyndns.sh script:
#      network_prefix="131.174"    => allows updating all ip addresses
#      dyndns_url="http://www.cs.ru.nl/lab/dyndns/"
#  - in server php script
#      $network_prefix="131.174";
    
# note for my own pi's I use   -> so that they can be used also at other places and with hostnames
#  - in client dyndns.sh script:
#      network_prefix=".*"    => allows updating all ip addresses
#      dyndns_url="http://hidden/url"
#  - in server php script
#      $network_prefix="";  # no prefix, allow any ip to be set in this database
#      $verify_mac=false; # if set to false you can also use a hostname or dns name to do lookup

# bash shell helpers 
#
#    url_dyndns_server="http://www.cs.ru.nl/lab/dyndns/"
#    getipfrommac() { curl "$url_dyndns_server?mac=$1&cmdline=1" 2>/dev/null; }
#    setmac2ip() { curl "$url_dyndns_server?mac=$1&ip=$2&cmdline=1" 2>/dev/null;  }
#
# usage:
#   $ setmac2ip c8:1f:66:bb:5f:2c 131.174.30.57
#   $ getipfrommac c8:1f:66:bb:5f:2c
#   131.174.30.57
#
# thus:
#    run setmac2ip when booting machine after getting ip
#    then othermachines can use the mac address to get ip of that machine  







$formdata = <<<HERE
<h1> Lookup ip using mac address </h1>
<form action="index.php" method="get">
  Mac address:  <input type="text" name="mac"><br>
  <br/>
  <input type="submit" value="Lookup IP">
</form> 
HERE;


$via_form = true;

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
function errorexit($msg) {
    global $formdata,$via_form,$print_all_data,$datafile;
    if ( $via_form ) {
       echo "<p style='color:#FF0000'>$msg</p>";
       echo "$formdata"; 
    } else{
       echo $msg;
    }
    if ( $print_all_data ) { 
        $oldstring = file_get_contents($datafile);
        $data= unserialize( $oldstring );
      if ( $via_form ) {
           echo "<br>"; 
           echo "<br>"; 
           var_dump($data);
      } else {     
           echo "\n";  
           echo "\n";  
           print_r($data);
      }
    }
    if ( $via_form ) {
       echo "</center></html>"; 
    }   
    exit(0);
}


$mac= $_GET["mac"];
$ip= $_GET["ip"];

if ( $_GET["cmdline"] ) $via_form =false;
if ( $via_form )  echo "<html><br/><br/><br/><center>"; 




if ( ! $mac    ) {
    errorexit("missing mac");
}    


if ( $verify_mac ) {
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
    if (  substr( $ip, 0, strlen($network_prefix)  ) != "$network_prefix"  ) {
        errorexit("invalid ip, must be within network $network_prefix.*.*");
    } 
}  





$oldstring = file_get_contents($datafile);
$data= unserialize( $oldstring );

if (  ! $ip  ) {
  #get
  $value=$data["$mac"];
  $pieces = explode(";", $value);
  $ip=$pieces[0];
  $timestamp=$pieces[1];
  if ( $via_form ) {
      echo "$formdata"; 
      if  ( $value ) {
         echo "<br><br><h2> Result:   <br><br>  $ip  </br>at</br> $timestamp</h2>"; 
      } else {
         echo "<br><br><h2> Result:   <br><br>  nothing found</h2>";
      }     
  } else {
      if  ( ! $value ) $ip="nothing found";
      echo $ip; # so can be directly used
  }
  
} else {
  #set
  $timestamp=date("Y-m-d H:i:s");
  $data["$mac"]="$ip;$timestamp";


  $newstring = serialize( $data ); 
  file_put_contents($datafile, $newstring, LOCK_EX);
  
  if ( $via_form ) {
      echo "$formdata"; 
      echo "<br><br><h2> Update:   <br><br> mac=$mac to ip=$ip</h2>"; 
  } else {
      echo "update mac=$mac to ip=$ip";
  }
}


if ( $print_all_data ) { 
  if ( $via_form ) {
       echo "<br>"; 
       echo "<br>"; 
       var_dump($data);
  } else {     
       echo "\n";  
       echo "\n";  
       print_r($data);
  }
}

if ( $via_form ) echo "</center></html>"; 



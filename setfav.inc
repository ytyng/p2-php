<?php
// p2 - ���C�ɃX���֌W�̏����X�N���v�g

require_once("./filectl_class.inc");
require_once("./datactl.inc");

//==================================================================
// �ϐ�
//==================================================================
if($_GET['setfav']=="0" or $_GET['setfav']=="1"){
	$favbool=$_GET['setfav'];
}

//==================================================================
// key.idx
//==================================================================

//idxfile�̃p�X�����߂�
$datdir_host=datdirOfHost($_GET['host']);
$idxfile="$datdir_host/{$_GET['bbs']}/{$_GET['key']}.idx";

//�f�B���N�g����������΍��
//FileCtl::mkdir_for($idxfile);

//�f�[�^������Ȃ�ǂݍ����
if( is_readable($idxfile) ){
	$lines=@file($idxfile);
	$data = explode("<>", $lines[0]);
}

//�l�ݒ�
$s="$data[0]<>{$_GET['key']}<>$data[2]<>$data[3]<>$data[4]<>$data[5]<>{$favbool}<>$data[7]<>$data[8]<>$data[9]";
/*	
//��������
$fp=fopen($idxfile,"w") or die("Error: $idxfile ���X�V�ł��܂���ł���");
fputs($fp,$s);
fclose($fp);
*/

//==================================================================
// favlist.idx�ɏ�������
//==================================================================

//================================================
// �ǂݍ���
//================================================

//favlist�t�@�C�����Ȃ���ΐ���
FileCtl::make_datafile($favlistfile, $favlist_perm);

//favlist�ǂݍ���;
$favlines= @file($favlistfile);

//================================================
// ����
//================================================

//�ŏ��ɏd���v�f���폜
if($favlines){
	$i=-1;
	unset($neolines);
	foreach($favlines as $favl){
		$i++;
		$lar = explode("<>", $favl);
		if($lar[1]==$_GET['key']){ //�d�����
			$before_line_num=$i;
			continue;
		}elseif(! $lar[1]){ //key�̂Ȃ����͕̂s���f�[�^
			continue;
		}else{
			$neolines[]=$favl;
		}
	}
}

//�V�K�f�[�^�ݒ�
if($_GET['setfav']){
	$newdata="$data[0]<>{$_GET['key']}<>$data[2]<>$data[3]<>$data[4]<>$data[5]<>{$favbool}<>$data[7]<>$data[8]<>$data[9]<>{$_GET['host']}<>{$_GET['bbs']}\n";
}
	
if($_GET['setfav']==1 or $_GET['setfav']=="top"){
	$after_line_num=0;
	
}elseif($_GET['setfav']=="up"){
	$after_line_num=$before_line_num-1;
	if($after_line_num<0){$after_line_num=0;}
	
}elseif($_GET['setfav']=="down"){
	$after_line_num=$before_line_num+1;
	if( $after_line_num >= sizeof($neolines) ){$after_line_num="bottom";}
	
}elseif($_GET['setfav']=="bottom"){
	$after_line_num="bottom";
}

//================================================
//��������
//================================================
$fp = @fopen($favlistfile,"wb") or die("Error: $favlistfile ���X�V�ł��܂���ł���");

if($neolines){
	$i=0;
	foreach($neolines as $l){
		if($i===$after_line_num){fputs($fp, $newdata);}
		fputs($fp, $l);
		$i++;
	}
	if($after_line_num==="bottom"){fputs($fp, $newdata);}
	//�u$after_line_num=="bottom"�v���ƌ듮�삷��B
}else{
	fputs($fp, $newdata);
}
fclose($fp);

//================================================
// ���C�ɃX�����L
//================================================

if($join_favrank){
	if($_GET['setfav']=="0"){
		$act="out";
	}elseif($_GET['setfav']=="1"){
		$act="add";
	}else{
		return;
	}
	$itaj = getItaName($_GET['host'], $_GET['bbs']);
	$post = array("host" => $_GET['host'], "bbs" => $_GET['bbs'], "key" => $_GET['key'], "ttitle" => $data[0], "ita" => $itaj, "act" => $act);
	postFavRank($post);
}

//================================================
// postFavRank -- ���C�ɃX�����L�Ń|�X�g����֐�
//================================================

function postFavRank($post){
	global $p2version, $favrank_url, $proxy;

	$method="POST";
	$httpua="Monazilla/1.00 p2/".$p2version;
	
	$URL=parse_url($favrank_url); //URL����
	if (isset($URL['query'])) { //�N�G���[
	    $URL['query'] = "?".$URL['query'];
	} else {
	    $URL['query'] = "";
	}

	//�v���L�V
	if($proxy['use']){
		$send_host=$proxy['host'];
		$send_port=$proxy['port'];
		$send_path=$url;
	}else{
		$send_host=$URL['host'];
		$send_port=$URL['port'];
		$send_path=$URL['path'].$URL['query'];
	}
	
	if (!$send_port){$send_port = 80;}//�f�t�H���g��80
	
	$request = $method." ".$send_path." HTTP/1.0\r\n";
	$request .= "Host: ".$URL['host']."\r\n";
	$request .= "User-Agent: ".$httpua."\r\n";
	$request .= "Connection: Close\r\n";

	/* POST�̎��̓w�b�_��ǉ����Ė�����URL�G���R�[�h�����f�[�^��Y�t */
	if (strtoupper($method) == "POST") {
	    while (list($name, $value) = each($post)) {
	        $POST[] = $name."=".urlencode($value);
	    }
	    $postdata = implode("&", $POST);
	    $request .= "Content-Type: application/x-www-form-urlencoded\r\n";
	    $request .= "Content-Length: ".strlen($postdata)."\r\n";
	    $request .= "\r\n";
	    $request .= $postdata;
	} else {
	    $request .= "\r\n";
	}

	/* WEB�T�[�o�֐ڑ� */
	$fp = fsockopen($send_host, $send_port, $errno, $errstr, 3);
	if(!$fp){
		//echo "�T�[�o�ڑ��G���[: $errstr ($errno)<br>\n";
		//echo "p2 info: {$favrank_url} �ɐڑ��ł��܂���ł����B<br>";
		return false;
	}else{
		fputs($fp, $request);
		/*
		while (!feof($fp)){
			if($start_here){
				echo $body = fread($fp,512000);
			}else{
				$l = fgets($fp,128000);
				if($l=="\r\n"){
					$start_here=true;
				}
			}
		}
		*/
		fclose ($fp);
		return true;
		//return $body;
	}
}

?>
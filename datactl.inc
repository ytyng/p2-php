<?php
// p2 - �f�[�^�R���g���[���n�̊֐��Q

require_once("filectl_class.inc");
require_once './p2util.class.php';	// p2�p�̃��[�e�B���e�B�N���X

/**
 * subject.txt���_�E�����[�h���ĕۑ�����
 */
function subjectDownload($url, $subjectfile)
{
	global $_conf, $datdir, $_info_msg_ht, $p2version, $ext_win_target, $fsockopen_time_limit, $proxy;

	$perm = (isset($_conf['dl_perm'])) ? $_conf['dl_perm'] : 0606;
	
	if (file_exists($subjectfile)) {
		if ($_GET['norefresh'] or isset($_GET['submit_kensaku']) || isset($_POST['submit_kensaku'])) { // ����
			return;
		}
		$modified = gmdate("D, d M Y H:i:s", filemtime($subjectfile))." GMT";
	} else {
		$modified = false;
	}

	if (extension_loaded('zlib') and strstr($url, ".2ch.net")){
		$headers="Accept-Encoding: gzip\r\n";
	}

	// ������΂�livedoor�ړ]�ɑΉ��B�Ǎ����livedoor�Ƃ���B
	$url = P2Util::adjustHostJbbs($url);

	//DL
	include_once("./wap.inc");
	$wap_ua = new UserAgent;
	$wap_ua->setAgent("Monazilla/1.00 p2/".$p2version);
	$wap_ua->setTimeout($fsockopen_time_limit);
	$wap_req = new Request;
	$wap_req->setUrl($url);
	$wap_req->setModified($modified);
	$wap_req->setHeaders($headers);
	if($proxy['use']){
		$wap_req->setProxy($proxy['host'], $proxy['port']);
	}
	$wap_res = $wap_ua->request($wap_req);
	
	if ($wap_res->is_error()) {
		$url_t = throughIme($wap_req->url);
		$_info_msg_ht .= "<div>Error: {$wap_res->code} {$wap_res->message}<br>";
		$_info_msg_ht .= "p2 info: <a href=\"{$url_t}\"{$ext_win_target}>{$wap_req->url}</a> �ɐڑ��ł��܂���ł����B</div>";
	}else{
		$body = $wap_res->content;
	}
	
	// �� DL�������� ���� �X�V����Ă�����
	if ($wap_res->is_success() && $wap_res->code != "304") {
		
		// ������΂Ȃ�EUC��SJIS�ɕϊ�
		if (strstr($subjectfile, $datdir."/jbbs.shitaraba.com") || strstr($subjectfile, $datdir."/jbbs.livedoor.com") || strstr($subjectfile, $datdir."/jbbs.livedoor.jp")) {
			include_once("./strctl_class.inc");
			$body = StrCtl::p2EUCtoSJIS($body);
		}
		
		$fp = @fopen($subjectfile,"wb") or die("Error: {$subjectfile} ���X�V�ł��܂���ł���");
		fwrite($fp, $body);
		fclose($fp);
		chmod($subjectfile, $perm);
	}
	
	return $wap_res;
}

/**
 * �t�@�C�����_�E�����[�h���ĕۑ�����
 */
function fileDownload($url, $localfile, $disp_error = 1)
{
	global $_conf, $_info_msg_ht, $ext_win_target, $fsockopen_time_limit, $proxy;

	$perm = (isset($_conf['dl_perm'])) ? $_conf['dl_perm'] : 0606;
	
	if (file_exists($localfile)) {
		$modified = gmdate("D, d M Y H:i:s", filemtime($localfile))." GMT";
	} else {
		$modified = false;
	}

	// DL
	include_once("./wap.inc");
	$wap_ua = new UserAgent;
	$wap_ua->setTimeout($fsockopen_time_limit);
	$wap_req = new Request;
	$wap_req->setUrl($url);
	$wap_req->setModified($modified);
	if ($proxy['use']) {
		$wap_req->setProxy($proxy['host'], $proxy['port']);
	}
	$wap_res = $wap_ua->request($wap_req);
	
	if ($wap_res->is_error() && $disp_error) {
		$url_t = throughIme($wap_req->url);
		$_info_msg_ht .= "<div>Error: {$wap_res->code} {$wap_res->message}<br>";
		$_info_msg_ht .= "p2 info: <a href=\"{$url_t}\"{$ext_win_target}>{$wap_req->url}</a> �ɐڑ��ł��܂���ł����B</div>";
	}
	
	// �X�V����Ă�����
	if ($wap_res->is_success() && $wap_res->code != "304") {
		$fdat = fopen($localfile, "wb") or die("Error: {$localfile} ���X�V�ł��܂���ł���");
		fwrite($fdat, $wap_res->content);
		fclose($fdat);
		chmod($localfile, $perm);
	}

	return $wap_res;
}

//===============================================
// �_�E�����[�hURL����L���b�V���t�@�C���p�X��Ԃ�
//===============================================
function cacheForDL($url){
	global $prefdir;
	$URL=parse_url($url); //URL����
	$cachefile = $prefdir."/p2_cache/{$URL['host']}{$URL['path']}";
	FileCtl::mkdir_for($cachefile);
	return $cachefile;
}

//===============================================
// �z�X�g����N�b�L�[�t�@�C���p�X��Ԃ��֐�
//===============================================
function cacheForCookie($host){
	global $prefdir;
	$cachefile = $prefdir."/p2_cookie/{$host}/p2_cookie.txt";
	FileCtl::mkdir_for($cachefile);
	return $cachefile;
}

//===============================================
// host��bbs�������Ԃ��֐�
//===============================================
function getItaName($host, $bbs){
	global $prefdir, $ita_names, $p2_perm;
	
	if(! isset($ita_names["$host/$bbs"]) ){
		$datdir_host = datdirOfHost($host);
		$p2_setting_txt = $datdir_host."/".$bbs."/p2_setting.txt";
		$p2_setting_cont = FileCtl::get_file_contents($p2_setting_txt);
		if($p2_setting_cont){$p2_setting = unserialize($p2_setting_cont);}
		$ita_names["$host/$bbs"] = $p2_setting['itaj'];
	}

	/* ��Long�̎擾
	if(! $p2_setting['itaj']){ //itaj���Z�b�g��2ch pink �Ȃ�SETTING.TXT��ǂ�ŃZ�b�g
		if( P2Util::isHost2chs($host) ){
			$tempfile=$prefdir."/SETTING.TXT.temp";
			fileDownload("http://{$host}/{$bbs}/SETTING.TXT", $tempfile);
			//$setting=getHttpContents("http://{$host}/{$bbs}/SETTING.TXT", "", "GET", "", array(""), $httpua="p2");
			$setting=file($tempfile);
			if(file_exists($tempfile)){ unlink($tempfile); }
			if($setting){
				foreach($setting as $sl){
					$sl=trim($sl);
					if(preg_match("/^BBS_TITLE=(.+)/", $sl, $matches)){
						$p2_setting['itaj']=$matches[1];
					}
				}
				if($p2_setting['itaj']){
					FileCtl::make_datafile($p2_setting_txt, $p2_perm);
					if($p2_setting){$p2_setting_cont = serialize($p2_setting);}
					if($p2_setting_cont){
						$fp = fopen($p2_setting_txt, "w") or die("Error: $p2_setting_txt ���X�V�ł��܂���ł���");
						fputs($fp, $p2_setting_cont);
						fclose($fp);
					}
				}
			}
		}
	}
	*/
	
	return $ita_names["$host/$bbs"];
}

//===============================================
// host����dat�̕ۑ��f�B���N�g����Ԃ��֐�
//===============================================
function datdirOfHost($host){
	global $datdir;
	if( P2Util::isHost2chs($host) ){ // 2channel or bbspink
		$datdir_host = $datdir."/2channel";
	}elseif( P2Util::isHostMachiBbs($host) ){ //machibbs.com
		$datdir_host = $datdir."/machibbs.com";
	}else{
		$datdir_host = $datdir."/".$host;
	}
	return $datdir_host;
}

//===============================================
// key.idx �� data ���L�^����֐�
//===============================================
function setKeyIdx($keyidx, $data){
	global $key_perm;
	
	FileCtl::make_datafile($keyidx, $key_perm);
	$fp=fopen($keyidx, "w") or die("Error: {$keyidx} ���X�V�ł��܂���ł���");
	fputs($fp, $data);
	fclose($fp);
}

//===============================================
// �Q�[�g��ʂ����߂�URL�ϊ��֐�
//===============================================
function throughIme($url){
	global $through_ime, $p2ime_url;
	
	if($through_ime=="2ch"){
		$purl = parse_url($url);
		$url_r = $purl[scheme] . "://ime.nu/" . $purl[host] . $purl[path];
	}elseif($through_ime=="p2"){
		$url_r = $p2ime_url . "?url=" . $url;
	}elseif($through_ime=="p2m"){
		$url_r = $p2ime_url . "?m=1&amp;url=" . $url;
	}else{
		$url_r = $url;
	}
	return $url_r;
}


//===============================================
// ���X�g�̃i�r�͈͂�Ԃ��֐�
//===============================================
function getListNaviRange($disp_from, $disp_range, $disp_all_num){

	$disp_end=0;
	$disp_navi=array();

	if(!$disp_all_num){
		$disp_navi['from']=0;
		$disp_navi['end']=0;
		$disp_navi['all_once']=true;
		$disp_navi['mae_from']=1;
		$disp_navi['tugi_from']=1;
		return $disp_navi;
	}	


	$disp_navi['from']=$disp_from;
	
	$disp_range = $disp_range-1;
	
	if($disp_navi['from'] > $disp_all_num){ //from���z����
		$disp_navi['from'] = $disp_all_num - $disp_range;
		if($disp_navi['from'] < 1){
			$disp_navi['from'] = 1;
		}
		$disp_navi['end'] = $disp_all_num;
					
	}else{//from �z���Ȃ�
		if($disp_navi['from'] + $disp_range > $disp_all_num){ //end �z����
			$disp_navi['end'] = $disp_all_num;
			if($disp_navi['from'] == 1){
				$disp_navi['all_once'] = true;
			}
		}else{// end �z���Ȃ�
			$disp_navi['end'] = $disp_from + $disp_range;
		}
	}
		
	$disp_navi['mae_from'] = $disp_from -1 -$disp_range;
	if($disp_navi['mae_from'] < 1){
		$disp_navi['mae_from'] = 1;
	}	
	$disp_navi['tugi_from'] = $disp_navi['end'] +1;


	if($disp_navi['from']==$disp_navi['end']){
		$range_on_st = $disp_navi['from'];
	}else{
		$range_on_st = "{$disp_navi['from']}-{$disp_navi['end']}";
	}
	$disp_navi['range_st']="{$range_on_st}/{$disp_all_num} ";


	return $disp_navi;
	
}

//�p�[�~�b�V�������ӊ��N================
function datadir_writable_check($aDir)
{
	global $_info_msg_ht, $data_dir_perm;
	if( ! is_dir($aDir) ){
		$_info_msg_ht .= <<<EOP
<p class="infomsg">����: �f�[�^�ۑ��p�f�B���N�g��������܂���B<br>
{$aDir}<br>
EOP;
		if( is_dir( dirname( realpath($aDir) ) ) && is_writable( dirname( realpath($aDir) ) )){
			$_info_msg_ht .="�f�B���N�g���̎����쐬�����݂܂�...<br>";
			if (mkdir($aDir, $data_dir_perm)) {
				$_info_msg_ht .="�f�B���N�g���̎����쐬���������܂����B</p>";
				chmod($aDir, $data_dir_perm);
			} else {
				$_info_msg_ht .= "�f�B���N�g���������쐬�ł��܂���ł����B<br>�蓮�Ńf�B���N�g�����쐬���A�p�[�~�b�V������ݒ肵�ĉ������B</p>";
			}
		}else{
				$_info_msg_ht .= "�f�B���N�g�����쐬���A�p�[�~�b�V������ݒ肵�ĉ������B</p>";
		}
	}elseif( ! is_writable($aDir) ){
		$_info_msg_ht .= <<<EOP
<p class="infomsg">����: �f�[�^�ۑ��p�f�B���N�g���ɏ������݌���������܂���B<br>
{$aDir}<br>
�f�B���N�g���̃p�[�~�b�V�������������ĉ������B</p>
EOP;
	}
}

?>
<?php
// p2 -  �T�u�W�F�N�g - �g�уw�b�_�\��
// for subject.php

//===============================================================
// HTML�\���p�ϐ�
//===============================================================
$newtime= date("gis");
$norefresh_q="&amp;norefresh=1";

// �y�[�W�^�C�g������URL�ݒ� ====================================
if($aThreadList->spmode=="taborn" or $aThreadList->spmode=="soko"){
	$ptitle_url = "{$subject_php}?host={$aThreadList->host}&amp;bbs={$aThreadList->bbs}{$k_at_a}";
}elseif($aThreadList->spmode=="res_hist"){
	$ptitle_url = "./read_res_hist.php{$k_at_q}#footer";
}elseif(! $aThreadList->spmode){
	$ptitle_url = "http://{$aThreadList->host}/{$aThreadList->bbs}/i/";
	if( preg_match("/www\.onpuch\.jp/", $aThreadList->host) ){$ptitle_url = $ptitle_url."index2.html";}
	if( preg_match("/livesoccer\.net/", $aThreadList->host) ){$ptitle_url = $ptitle_url."index2.html";}
	//match�o�^���head�Ȃ��ĕ������ق����悳���������A�������X�|���X������̂�����
}

// �y�[�W�^�C�g������HTML�ݒ� ====================================
if($aThreadList->spmode == "taborn"){
	$ptitle_ht = <<<EOP
	<a href="{$ptitle_url}"><b>{$aThreadList->itaj}</b></a>�i���ݒ��j
EOP;
}elseif($aThreadList->spmode == "soko"){
	$ptitle_ht = <<<EOP
	<a  href="{$ptitle_url}"><b>{$aThreadList->itaj}</b></a>�idat�q�Ɂj
EOP;
}elseif($ptitle_url){
	$ptitle_ht = <<<EOP
	<a  href="{$ptitle_url}"><b>{$aThreadList->ptitle}</b></a>
EOP;
}else{
	$ptitle_ht = <<<EOP
	<b>{$aThreadList->ptitle}</b>
EOP;
}

//�t�H�[��==================================================
if($aThreadList->bbs){
	$input_bbs_ht="<input type=\"hidden\" name=\"bbs\" value=\"{$aThreadList->bbs}\">";
}
if($aThreadList->host){
	$input_host_ht="<input type=\"hidden\" name=\"host\" value=\"{$aThreadList->host}\">";
}
if($aThreadList->spmode){
	$input_spmode_ht="<input type=\"hidden\" name=\"spmode\" value=\"{$aThreadList->spmode}\">";
}
$sb_form_hidden_ht =<<<EOP
		$k_input_ht
		$input_bbs_ht
		$input_host_ht
		$input_spmode_ht
EOP;

//�t�B���^���� ==================================================
if(!$aThreadList->spmode){
	$filter_form_ht = <<<EOP
<form method="GET" action="subject.php">
	$k_input_ht
	$sb_form_hidden_ht
	<input type="text" id="word" name="word" value="{$word}" size="12">
	<input type="submit" name="submit_kensaku" value="����">
</form>\n
EOP;
}

//��������==
if($mikke){
	$hit_ht="<div>\"{$word}\" {$mikke}hit!</div>";
}


//=================================================
//�w�b�_�v�����g
//=================================================
header_nocache();
header_content_type();
if($doctype){ echo $doctype;}
echo <<<EOP
<html>
<head>
	<meta name="ROBOTS" content="NOINDEX, NOFOLLOW">
	<title>{$aThreadList->ptitle}</title>
</head>
<body>
EOP;

echo $_info_msg_ht;
$_info_msg_ht="";

include("./sb_toolbar_k.inc");

echo $filter_form_ht;
echo $hit_ht;
echo "<hr>";
?>
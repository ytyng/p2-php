<?php
// p2 - �X���b�h��\������ �N���X PC�p

class ShowThreadPc extends ShowThread{
	
	var $quote_res_nums_checked; //�|�b�v�A�b�v�\�������`�F�b�N�ς݃��X�ԍ���o�^�����z��
	var $quote_res_nums_done; //�|�b�v�A�b�v�\�������L�^�ς݃��X�ԍ���o�^�����z��
	var $quote_check_depth; //���X�ԍ��`�F�b�N�̍ċA�̐[�� checkQuoteResNums()

	function ShowThreadPc($aThread){
		$this->thread = $aThread;
	}
	
	//============================================================================
	// Dat��HTML�ɕϊ��\�����郁�\�b�h
	//============================================================================	
	function datToHtml()
	{
		if (!$this->thread->resrange) {
			echo '<b>p2 error: {$this->resrange} is false at datToHtml() in threadread_class.inc</b>';
		}

		$start = $this->thread->resrange['start'];
		$to = $this->thread->resrange['to'];
		$nofirst = $this->thread->resrange['nofirst'];

		$status_title = $this->thread->itaj." / ".$this->thread->ttitle;
		$status_title = str_replace("'", "\'", $status_title);
		$status_title = str_replace('"', "\'\'", $status_title);
		$cont_ht = "<dl onMouseover=\"window.top.status='{$status_title}';\">";
		
		if(!$nofirst){ //1��\��
			$cont_ht .= $this->transRes($this->thread->datlines[0], 1);
		}

		for($i=$start; $i<=$to; $i++){
		
			if(!$nofirst and $i==1){continue;}
			if(!$this->thread->datlines[$i-1]){
				$this->thread->newline=$i;
				break;
			}
			$cont_ht .= $this->transRes($this->thread->datlines[$i-1], $i);
		}

		$cont_ht .= "</dl>\n";
		
		//$s2e=array($start, $i-1);
		//return $s2e;
		return $cont_ht;
	}


	//============================================================================
	// Dat���X��HTML���X�ɕϊ����郁�\�b�h
	// ���� - dat��1���C��, ���X�ԍ�
	//============================================================================
	function transRes($ares,$i)
	{
		global $STYLE, $mae_msg, $field, $res_filter, $word_fm, $quote_res_view;
		global $aborn_words, $ng_words, $ng_num, $newres_to_show;
		
		$tores="";
		$rpop="";
		$isNgName=false;
		$isNgMsg=false;
		
		$resar = $this->thread->explodeDatLine($ares);
		$name=$resar[0];
		$mail = $resar[1];
		$date_id = $resar[2];
		$msg=$resar[3];

		if($field){
			if(! $word_fm){return;}

			if($res_filter['field']=="name"){$target = StrCtl::p2SJIStoEUC($name);}
			elseif($res_filter['field']=="mail"){$target = StrCtl::p2SJIStoEUC($mail);}
			elseif($res_filter['field']=="id"){$target = StrCtl::p2SJIStoEUC($date_id);} //���ۂ͓��t���܂܂��
			elseif($res_filter['field']=="msg"){$target= StrCtl::p2SJIStoEUC($msg);}
			if($res_filter['match']=="on"){
				if(! @preg_match("/{$word_fm}/i", $target)){return;}
			}else{
				if(@preg_match("/{$word_fm}/i", $target)){return;}
			}
		}
		
		//���ځ[��`�F�b�N====================================
		$aborned_res .= "<dt id=\"r{$i}\" class=\"aborned\"><span>&nbsp;</span></dt>\n"; // ���O
		$aborned_res .= "<!-- <dd class=\"aborned\">&nbsp;</dd> -->\n"; // ���e

		//���ځ[��l�[��=================================
		if( $this->aborn($aborn_words['name'], $name, $i) ){
			return $aborned_res;
		}

		//���ځ[�񃁁[��=================================
		if( $this->aborn($aborn_words['mail'], $mail, $i) ){
			return $aborned_res;
		}
		
		//���ځ[��ID===================================
		if( $this->aborn($aborn_words['id'], $date_id, $i) ){
			return $aborned_res;
		}
		
		//���ځ[�񃁃b�Z�[�W==============================
		if( $this->aborn($aborn_words['msg'], $msg, $i) ){
			return $aborned_res;
		}

		//NG�l�[���`�F�b�N===============================
		if( $this->ngCheck($ng_words['name'], $name) ){
			$ng_num['name']++;
			$isNgName=true;
		}

		//NG���[���`�F�b�N===============================
		if( $this->ngCheck($ng_words['mail'], $mail) ){
			$ng_num['mail']++;
			$isNgMail=true;
		}

		//NGID�`�F�b�N===================================
		if( $this->ngCheck($ng_words['id'], $date_id) ){
			$ng_num['id']++;
			$isNgId=true;
		}

		//NG���b�Z�[�W�`�F�b�N===============================
		if( $a_ng_msg = $this->ngCheck($ng_words['msg'], $msg) ){
			$ng_num['msg']++;
			$isNgMsg=true;
		}
		
		//=============================================================
		// ���X���|�b�v�A�b�v�\��
		//=============================================================
		if($quote_res_view){
			$this->quote_check_depth = 0;
			$quote_res_nums = $this->checkQuoteResNums($i, $name, $msg);
			
			foreach($quote_res_nums as $rnv){
				if(! $this->quote_res_nums_done[$rnv]){
					$ds =$this->qRes( $this->thread->datlines[$rnv-1], $rnv );
					$onPopUp_at=" onMouseover=\"showResPopUp('q{$rnv}of{$this->thread->key}',event)\" onMouseout=\"hideResPopUp('q{$rnv}of{$this->thread->key}')\"";
					$rpop .=  "<dd id=\"q{$rnv}of{$this->thread->key}\" class=\"respopup\"{$onPopUp_at}><i>" . $ds . "</i></dd>\n";
					$this->quote_res_nums_done[$rnv]=true;
				}
			}
		}
		
		//=============================================================
		// �܂Ƃ߂ďo��
		//=============================================================
		
		$name = $this->transName($name); // ���OHTML�ϊ�
		$msg = $this->transMsg($msg, $i); //���b�Z�[�WHTML�ϊ�

		
		//NG���b�Z�[�W�ϊ�======================================
		if($isNgMsg){
			$msg=<<<EOMSG
<s class="ngword" onMouseover="document.getElementById('ngm{$ng_num['msg']}').style.display = 'block';">NG���[�h�F{$a_ng_msg}</s>
<div id="ngm{$ng_num['msg']}" style="display:none;">$msg</div>
EOMSG;
		}
		
		//NG�l�[���ϊ�======================================
		if($isNgName){
			$name=<<<EONAME
<s class="ngword" onMouseover="document.getElementById('ngn{$ng_num['name']}').style.display = 'block';">$name</s>
EONAME;
			$msg=<<<EOMSG
<div id="ngn{$ng_num['name']}" style="display:none;">$msg</div>
EOMSG;
		
		//NG���[���ϊ�======================================
		}elseif($isNgMail){
			$mail=<<<EOMAIL
<s class="ngword" onMouseover="document.getElementById('ngn{$ng_num['mail']}').style.display = 'block';">$mail</s>
EOMAIL;
			$msg=<<<EOMSG
<div id="ngn{$ng_num['mail']}" style="display:none;">$msg</div>
EOMSG;

		//NGID�ϊ�======================================
		}elseif($isNgId){
			$date_id=<<<EOID
<s class="ngword" onMouseover="document.getElementById('ngn{$ng_num['id']}').style.display = 'block';">$date_id</s>
EOID;
			$msg=<<<EOMSG
<div id="ngn{$ng_num['id']}" style="display:none;">$msg</div>
EOMSG;

		}
	
		/*
		//�u��������V���v�摜��}��========================
		if($i == $this->thread->newline){
			$tores .=<<<EOP
				<div><img src="img/image.png" alt="�V�����X" border="0" vspace="4"></div>
EOP;
		}
		*/

		if($this->thread->onthefly){
			$newres_to_show=true;
			$tores .= "<dt id=\"r{$i}\"><span class=\"ontheflyresorder\">{$i}</span> �F"; //�ԍ��i�I���U�t���C���j
		}elseif($i>=$this->thread->newline){
			$newres_to_show=true;
			$tores .= "<dt id=\"r{$i}\"><font color=\"{$STYLE['read_newres_color']}\">{$i}</font> �F"; //�ԍ��i�V�����X���j
		}else{
			$tores .= "<dt id=\"r{$i}\">{$i} �F"; //�ԍ�			
		}
		$tores .= "<span class=\"name\"><b>{$name}</b></span>�F"; //���O
		
		if($mail){ //���[��
			if( strstr($mail, "sage") && $STYLE['read_mail_sage_color'] ){
				$tores .= "<span class=\"sage\">{$mail}</span> �F";
			}elseif( $STYLE['read_mail_color'] ){
				$tores .= "<span class=\"mail\">{$mail}</span> �F";
			}else{
				$tores .= $mail." �F";
			}
		}

		$tores .= $date_id."</dt>\n"; //���t��ID
		$tores .= $rpop; //���X�|�b�v�A�b�v�p���p
		$tores .= "<dd>{$msg}<br><br></dd>\n"; //���e
		
		return $tores;
	}


	//=========================================================
	// >>1 ��\�����郁�\�b�h (���p�|�b�v�A�b�v�p)
	//=========================================================
	function quoteOne(){
		global $quote_res_view;
		
		if(! $quote_res_view){
			return false;
		}
		
		$dummy_msg="";
		$this->quote_check_depth=0;
		$quote_res_nums = $this->checkQuoteResNums(0, "1", $dummy_msg);
		foreach($quote_res_nums as $rnv){
			if(! $this->quote_res_nums_done[$rnv]){
				if($this->thread->ttitle){
					$ds = "<b>{$this->thread->ttitle}</b><br><br>";
				}
				$ds .= $this->qRes( $this->thread->datlines[$rnv-1], $rnv );
				$onPopUp_at=" onMouseover=\"showResPopUp('q{$rnv}of{$this->thread->key}',event)\" onMouseout=\"hideResPopUp('q{$rnv}of{$this->thread->key}')\"";
				$rpop .= "<div id=\"q{$rnv}of{$this->thread->key}\" class=\"respopup\"{$onPopUp_at}><i>" . $ds . "</i></div>\n";
				$this->quote_res_nums_done[$rnv]=true;
			}
		}
		$res1['q'] = $rpop;
		
		$m1 = "&gt;&gt;1";
		$res1['body'] = $this->transMsg($m1, 1);
		return $res1;
	}

	//============================================================================
	// qRes -- ���X���pHTML�̃��\�b�h
	//============================================================================
	function qRes($ares,$i){

		$resar = $this->thread->explodeDatLine($ares);
		$name=$resar[0];
		$name=$this->transName($name);
		$msg=$resar[3];
		$msg=$this->transMsg($msg, $i); //���b�Z�[�W�ϊ�
		$mail = $resar[1];
		$date_id = $resar[2];
		
		// $tores�ɂ܂Ƃ߂ďo��
		$tores = "$i �F"; //�ԍ�			
		$tores .= "<b>$name</b> �F"; //���O
		if($mail){$tores .= $mail." �F";} //���[��
		$tores .= $date_id."<br>"; //���t��ID
		$tores .= $msg."<br>\n"; //���e
		
		return $tores;
	}
	
	//============================================================================
	// transName -- ���O��HTML�p�ɕϊ����郁�\�b�h
	//============================================================================
	function transName($name)
	{
		global $ext_win_target, $_conf, $quote_res_view;
		$nameID="";

		if( preg_match("/(.*)(��.*)/", $name, $matches) ){ //���O
			$name=$matches[1];
			$nameID=$matches[2];
		}

		// 1�������N��
		if($quote_res_view){
			$onPopUp_at=" onMouseover=\"showResPopUp('q\\1of{$this->thread->key}',event)\" onMouseout=\"hideResPopUp('q\\1of{$this->thread->key}')\"";
			$name && $name=preg_replace("/([0-9]+)/","<a href=\"{$_conf['read_php']}?host={$this->thread->host}&amp;bbs={$this->thread->bbs}&amp;key={$this->thread->key}&amp;ls=\\1\"{$ext_win_target}{$onPopUp_at}>\\1</a>",$name, 1);
		}
		
		if($nameID){$name = $name . $nameID;}
		
		$name=$name." "; //�����������
		
		/*
		$b = unpack('C*', $name);
		$n = count($b);
		if((0x80 <= $b[$n] && $b[$n] <= 0x9F) or (0xE0 <= $b[$n] && $b[$n] <= 0xEF)){
			$name=$name." "; 
		}
		*/
		
		return $name;
	}

	
	//============================================================================
	// transMsg --  dat�̃��X���b�Z�[�W��HTML�\���p���b�Z�[�W�ɕϊ����郁�\�b�h
	// string transMsg(string str)
	//============================================================================	
	function transMsg($msg, $mynum)
	{
		global $_conf, $subject_php, $preview_thumbnail, $brocra_checker, $ext_win_target, $bbs_win_target, $through_ime;
		global $field, $res_filter, $word_fm, $iframe_popup, $iframe_popup_delay, $p2ime_url;
		
		//2ch���`����dat
		if($this->thread->dat_type == "2ch_old"){
			$msg = str_replace("���M", ",", $msg);
			$msg = preg_replace("/&amp([^;])/", "&\\1", $msg);
		}

		// >>1�̃����N����������O��
		// <a href="../test/read.cgi/accuse/1001506967/1" target="_blank">&gt;&gt;1</a>
		$msg=preg_replace("/<a href=\"\.\.\/test\/read\.cgi\/{$this->thread->bbs}\/{$this->thread->key}\/([-0-9]+)\" target=\"_blank\">&gt;&gt;([-0-9]+)<\/a>/","&gt;&gt;\\1", $msg);
		
		//�t�B���^�F����
		if($field and $res_filter['field']=="msg" && $word_fm && $res_filter['match']=="on"){
			$msg_euc = StrCtl::p2SJIStoEUC($msg);
			$msg_euc = @preg_replace("/{$word_fm}/i", "<b class='filtering'>\\0</b>", $msg_euc);
			$msg = StrCtl::p2EUCtoSJIS($msg_euc);
		}

		// target="_blank" �� ���[�U�ݒ�{$ext_win_target}�ɕϊ�
		$msg=preg_replace("/(<a href=.+?) target=\"_blank\">/", "\\1{$ext_win_target}>", $msg);
	
		// >>1, >1, ��1, ����1�����p���X�|�b�v�A�b�v�����N��
		$msg=preg_replace_callback("/(&gt;|��)?(&gt;|��)([0-9- ,=&.]|�A)+/", array($this, 'quote_res_callback'), $msg);
	
		// FTP�����N�̗L����
		$msg=preg_replace("/ftp:\/\/[-_.!~*'a-zA-Z0-9;\/?:@&=+\$,%#]+/","<a href=\"\\0\"{$ext_win_target}>\\0</a>", $msg);

		// daap�����N�̗L����
		$msg=preg_replace("/daap:\/\/[-_.!~*'a-zA-Z0-9;\/?:@&=+\$,%#]+/","<a href=\"\\0\">\\0</a>", $msg);
		
		// �ih�������܂߂��jURL�����N�̗L����
		$msg = preg_replace("/([^f])(h?t?)(tps?:\/\/[-_.!~*'a-zA-Z0-9;\/?:@&=+\$,%#]+)/","\\1<a href=\"ht\\3\"{$ext_win_target}>\\2\\3</a>", $msg);
		$msg = preg_replace("/&gt;\"{$ext_win_target}>(.+)&gt;<\/a>/","\"{$ext_win_target}>\\1</a>&gt;", $msg); //������&gt;�i>�j�����O���Ă���
		
		// �T�[�o�������N��p2�\����
		// 2ch bbspink
		// http://choco.2ch.net/test/read.cgi/event/1027770702/
		$msg = preg_replace_callback("/<a href=\"http:\/\/([^\/]+\.(2ch\.net|bbspink\.com))\/test\/read\.cgi\/([^\/]+)\/([0-9]+)(\/)?([^\/]+)?\"{$ext_win_target}>(h?t?tp:\/\/([^\/]+(\.2ch\.net|\.bbspink\.com))\/test\/read\.cgi\/([^\/]+)\/([0-9]+)(\/)?([^\/]+)?)<\/a>/", array($this, 'link2ch_callback'), $msg);
			
		// �܂�BBS / JBBS��������� 
		// http://kanto.machibbs.com/bbs/read.pl?BBS=kana&KEY=1034515019
		// http://jbbs.livedoor.jp/study/bbs/read.cgi?BBS=389&KEY=1036227774&LAST=100
		$ande = "(&|&amp;)";
		$msg = preg_replace_callback("{<a href=\"http://(([^/]+\.machibbs\.com|[^/]+\.machi\.to|jbbs\.livedoor\.jp|jbbs\.livedoor\.com|jbbs\.shitaraba\.com)(/[^/]+)?)/bbs/read\.(pl|cgi)\?BBS=([^&]+)(&|&amp;)KEY=([0-9]+)((&|&amp;)START=([0-9]+))?((&|&amp;)END=([0-9]+))?[^\"]*\"{$_conf['ext_win_target_at']}>(h?t?tp://[^<>]+)</a>}", array($this, 'linkMachi_callback'), $msg);
		$msg = preg_replace_callback("{<a href=\"http://(jbbs\.livedoor\.jp|jbbs\.livedoor\.com|jbbs\.shitaraba\.com)/bbs/read\.cgi/(\w+)/(\d+)/(\d+)/((\d+)?-(\d+)?)?[^\"]*?\"{$_conf['ext_win_target_at']}>(h?t?tp://[^<>]+)</a>}", array($this, 'linkJBBS_callback'), $msg);
		//$msg=preg_replace("/&(amp;)?ls=-/", "", $msg);// ��͈͎̔w��͏���
		
		// 2ch��bbspink�̔�
		$msg = preg_replace("/<a href=\"http:\/\/([^\/]+\.(2ch\.net|bbspink\.com))\/([^\/]+)\/\"{$ext_win_target}>h?t?tp:\/\/([^\/]+(\.2ch\.net|\.bbspink\.com))\/([^\/]+)\/<\/a>/", "\\0 [<a href=\"{$subject_php}?host=\\1&amp;bbs=\\3\" target=\"subject\">��p2�ŊJ��</a>]", $msg);
		
		//2ch��bbspink�̉ߋ����O
		$msg = preg_replace_callback("/<a href=\"(http:\/\/([^\/]+\.(2ch\.net|bbspink\.com))(\/[^\/]+)?\/([^\/]+)\/kako\/\d+(\/\d+)?\/(\d+)).html\"{$ext_win_target}>h?t?tp:\/\/[^\/]+(\.2ch\.net|\.bbspink\.com)(\/[^\/]+)?\/[^\/]+\/kako\/\d+(\/\d+)?\/\d+.html<\/a>/", array($this, 'link2chkako_callback'), $msg);
		
		// �u���N���`�F�b�J
		if ($brocra_checker['use']) {
			$msg = preg_replace("/<a href=\"(s?https?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+)\"{$ext_win_target}>(s?h?t?tps?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+)<\/a>/","<a href=\"\\1\"{$ext_win_target}>\\2</a> [<a href=\"{$brocra_checker['url']}?{$brocra_checker['query']}=\\1\"{$ext_win_target}>�`�F�b�N</a>]", $msg);
		}
	
		// �摜URL�����N���T���l�C����
		if ($preview_thumbnail) {
			$msg = preg_replace_callback("/<a href=\"(s?https?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+\.([jJ][pP][eE]?[gG]|[gG][iI][fF]|[pP][nN][gG]))\"{$ext_win_target}>(s?h?t?tps?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+\.([jJ][pP][eE]?[gG]|[gG][iI][fF]|[pP][nN][gG]))<\/a>/", array($this, 'view_img_callback') ,$msg);
		}
	
		// ime
		if($through_ime=="p2"){	//p2 ime
			$msg=preg_replace("/<a href=\"(s?https?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+)\"{$ext_win_target}>([^><]+)<\/a>/","<a href=\"{$p2ime_url}?url=\\1\"{$ext_win_target}>\\2</a>", $msg);
		}elseif($through_ime=="p2m"){	//p2 ime
			$msg=preg_replace("/<a href=\"(s?https?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+)\"{$ext_win_target}>([^><]+)<\/a>/","<a href=\"{$p2ime_url}?m=1&amp;url=\\1\"{$ext_win_target}>\\2</a>", $msg);
		}elseif($through_ime=="2ch"){ // ime.nu
			$msg=preg_replace("/<a href=\"(s?https?:\/\/)([-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+)\"{$ext_win_target}>([^><]+)<\/a>/","<a href=\"\\1ime.nu/\\2\"{$ext_win_target}>\\3</a>", $msg);
		}
	
		//HTML�|�b�v�A�b�v
		$ommouse_popup=" onMouseover=\"showHtmlPopUp('\\1',event,{$iframe_popup_delay})\" onMouseout=\"offHtmlPopUp()\"";
		if($iframe_popup==1){
			$msg=preg_replace("/<a href=\"(s?https?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+)\"{$ext_win_target}>([^><]+)<\/a>/","<a href=\"\\1\"{$ext_win_target}{$ommouse_popup}>\\2</a>", $msg);
		}elseif($iframe_popup==2){
			$msg=preg_replace("/<a href=\"(s?https?:\/\/[-_.!~*'()a-zA-Z0-9;\/?:@&=+\$,%#]+)\"{$ext_win_target}>([^><]+)<\/a>/","(<a href=\"\\1\"{$ext_win_target}{$ommouse_popup}>p</a>)<a href=\"\\1\"{$ext_win_target}>\\2</a>", $msg);		
}
		return $msg;
	}

	//=============================================================
	//�R�[���o�b�N���\�b�h
	//=============================================================
	
	//���p���X�ϊ�===========================
	function quote_res_callback($s){
		$rs = preg_replace_callback("/(&gt;|��)?(&gt;|��)?([0-9-]+)/", array($this, 'quote_res_devide_callback'), $s[0]);
		return $rs;
	}
	
	function quote_res_devide_callback($s)
	{
		global $_conf, $bbs_win_target, $iframe_popup, $iframe_popup_delay, $quote_res_view;
		
		$appointed_num=$s[3];
		$qsign="$s[1]$s[2]";
		
		if($appointed_num=="-"){
			return $s[0];
		}
		
		$read_url="{$_conf['read_php']}?host={$this->thread->host}&amp;bbs={$this->thread->bbs}&amp;key={$this->thread->key}&amp;ls={$appointed_num}";
		
		if( $iframe_popup and strstr($appointed_num, "-") ){
			$rs=<<<EOP
<a href="{$read_url}n"{$bbs_win_target} onMouseover="showHtmlPopUp('{$_conf['read_php']}?host={$this->thread->host}&amp;bbs={$this->thread->bbs}&amp;key={$this->thread->key}&amp;ls={$appointed_num}n&amp;renzokupop=true',event,{$iframe_popup_delay})" onMouseout="offHtmlPopUp()">{$qsign}{$appointed_num}</a>
EOP;
/*
		}elseif( $iframe_popup==2 and strstr($appointed_num, "-") ){
			$rs=<<<EOP
(<a href="{$read_url}n"{$bbs_win_target} onMouseover="showHtmlPopUp('{$_conf['read_php']}?host={$this->thread->host}&amp;bbs={$this->thread->bbs}&amp;key={$this->thread->key}&amp;ls={$appointed_num}n&amp;renzokupop=true',event,{$iframe_popup_delay})" onMouseout="offHtmlPopUp()">p</a>)<a href="{$read_url}n"{$bbs_win_target}>{$qsign}{$appointed_num}</a>
EOP;
*/
		}else{
			$qnum=$appointed_num+0;
			if($qnum > sizeof($this->thread->datlines)){ //�����߂��郌�X�͕ϊ����Ȃ�
				return $s[0];
			}
			if($quote_res_view){
				$onPopUp_at=<<<EOP
 onMouseover="showResPopUp('q{$qnum}of{$this->thread->key}',event)" onMouseout="hideResPopUp('q{$qnum}of{$this->thread->key}')"
EOP;
			}
			$rs=<<<EOP
<a href="{$read_url}"{$bbs_win_target}{$onPopUp_at}>{$qsign}{$appointed_num}</a>
EOP;
		}
		return $rs;
	}
	
	//2ch bbspink �������N===========================
	function link2ch_callback($s)
	{
		global $_conf, $bbs_win_target, $iframe_popup, $iframe_popup_delay;
		
		$read_url="{$_conf['read_php']}?host=$s[1]&amp;bbs=$s[3]&amp;key=$s[4]&amp;ls=$s[6]";
		
		if(!$iframe_popup){ //HTML�|�b�v�A�b�v�Ȃ�
			$rs=<<<EORS
		<a href="{$read_url}"{$bbs_win_target}>$s[7]</a>
EORS;

		}else{ //HTML�|�b�v�A�b�v����
			if( preg_match("/^[0-9-n]+$/", $s[6]) ){
				$ommouse_popup=" onMouseover=\"showHtmlPopUp('http://$s[1]/test/read.cgi/$s[3]/$s[4]$s[5]$s[6]',event,{$iframe_popup_delay})\" onMouseout=\"offHtmlPopUp()\"";
				if($iframe_popup==1){
					$rs=<<<EORS
			<a href="{$read_url}"{$bbs_win_target}{$ommouse_popup}>$s[7]</a>
EORS;
				}else{
					$rs=<<<EORS
			(<a href="{$read_url}"{$bbs_win_target}{$ommouse_popup}>p</a>)<a href="{$read_url}"{$bbs_win_target}>$s[7]</a>
EORS;
				}
				
			}else{
				$ommouse_popup=" onMouseover=\"showHtmlPopUp('{$read_url}&amp;one=true',event,{$iframe_popup_delay})\" onMouseout=\"offHtmlPopUp()\"";
				if($iframe_popup==1){
					$rs=<<<EORS
			<a href="{$read_url}"{$bbs_win_target}{$ommouse_popup}>$s[7]</a>
EORS;
				}else{
					$rs=<<<EORS
			(<a href="{$read_url}"{$bbs_win_target}{$ommouse_popup}>p</a>)<a href="{$read_url}"{$bbs_win_target}>$s[7]</a>
EORS;
				}
			}
		}
		
		return $rs;
	}
	
	// �܂�BBS / JBBS���������  �������N===========================
	function linkMachi_callback($s)
	{
		global $_conf, $bbs_win_target, $iframe_popup, $iframe_popup_delay;
	
	 	return "<a href=\"{$_conf['read_php']}?host={$s[1]}&amp;bbs={$s[4]}&amp;key={$s[6]}&amp;ls={$s[9]}-{$s[12]}\"{$bbs_win_target}>{$s[13]}</a>";
	}
	 
	// JBBS���������  �������N 2===========================
	function linkJBBS_callback($s)
	{
		global $_conf, $bbs_win_target, $iframe_popup, $iframe_popup_delay;
	
	 	return "<a href=\"{$_conf['read_php']}?host={$s[1]}/{$s[2]}&amp;bbs={$s[3]}&amp;key={$s[4]}&amp;ls={$s[5]}\"{$bbs_win_target}>{$s[8]}</a>";
	}
	 
	//2ch�ߋ����Ohtml=============================
	function link2chkako_callback($s)
	{
		global $_conf, $iframe_popup, $iframe_popup_delay, $bbs_win_target;
		/*
		$msg=preg_replace_callback("/<a href=\"(http:\/\/([^\/]+(\.2ch\.net|\.bbspink\.com))(\/[^\/]+)?\/([^\/]+)\/kako\/\d+(\/\d+)?\/(\d+)).html\"{$ext_win_target}>h?t?tp:\/\/[^\/]+(\.2ch\.net|\.bbspink\.com)(\/[^\/]+)?\/[^\/]+\/kako\/\d+(\/\d+)?\/\d+.html<\/a>/", array($this, 'link2chkako_callback'), $msg);
		*/
		$kakolog_uri = $s[1];
		$kakolog_uri_en = urlencode($kakolog_uri);
		$host = $s[2]; $bbs=$s[5]; $key=$s[7];
		$read_url = "{$_conf['read_php']}?host={$host}&amp;bbs={$bbs}&amp;key={$key}&amp;kakolog={$kakolog_uri_en}";
		$ommouse_popup = " onMouseover=\"showHtmlPopUp('{$read_url}&amp;one=true',event,{$iframe_popup_delay})\" onMouseout=\"offHtmlPopUp()\"";
		
		if(!$iframe_popup){ //HTML�|�b�v�A�b�v�Ȃ�
			$rs=<<<EOP
<a href="{$read_url}"{$bbs_win_target}>{$kakolog_uri}.html</a>
EOP;
		}else{ //HTML�|�b�v�A�b�v����
			if($iframe_popup==1){
				$rs=<<<EOP
<a href="{$read_url}"{$bbs_win_target}{$ommouse_popup}>{$kakolog_uri}.html</a>
EOP;
			}else{
				$rs=<<<EOP
(<a href="{$read_url}"{$bbs_win_target}{$ommouse_popup}>p</a>)<a href="{$read_url}"{$bbs_win_target}>{$kakolog_uri}.html</a>
EOP;
			}
		}

		return $rs;
	}
	
	//�摜�|�b�v�A�b�v�ϊ�==========================
	function view_img_callback($s){
		global $ext_win_target, $iframe_popup, $iframe_popup_delay, $pre_thumb_height, $pre_thumb_width;
		
		$ommouse_popup=" onMouseover=\"showHtmlPopUp('{$s[1]}',event,{$iframe_popup_delay})\" onMouseout=\"offHtmlPopUp()\"";
		$img_tag=<<<EOIMG
<img class="thumbnail" src="$s[1]" height="{$pre_thumb_height}" weight="{$pre_thumb_width}" hspace="4" vspace="4" align="middle">
EOIMG;
		
		if($iframe_popup==1){ //HTML�|�b�v�A�b�v����
			$rs =<<<EORS
			<a href="$s[1]"{$ext_win_target}{$ommouse_popup}>{$img_tag}{$s[3]}</a>
EORS;
		}elseif($iframe_popup==2){
			$rs =<<<EORS
			<a href="$s[1]"{$ext_win_target}{$ommouse_popup}>{$img_tag}</a><a  href="$s[1]"{$ext_win_target}>{$s[3]}</a>
EORS;
		}else{
			$rs =<<<EORS
			<a href="$s[1]"{$ext_win_target}>{$img_tag}{$s[3]}</a>
EORS;
		}
		
		return $rs;
	}


	//============================================================================
	// checkQuoteResNums -- HTML���b�Z�[�W���̈��p���X�̔ԍ����ċA�`�F�b�N���郁�\�b�h
	//============================================================================	
	function checkQuoteResNums($res_num, $name, $msg){
		//�ċA���~�b�^
		if($this->quote_check_depth>30){
			return array();
		}else{
			$this->quote_check_depth++;
		}
		
		$quote_res_nums = array();

		$name=preg_replace("/(��.*)/", "", $name, 1);
		
		if( preg_match("/[0-9]+/", $name, $matches) ){ //���O
			$a_quote_res_num=$matches[0];
			
			if($a_quote_res_num){
				$quote_res_nums[] = $a_quote_res_num;
		
				if($a_quote_res_num != $res_num){ //�������g�̔ԍ��Ɠ���łȂ���΁A
					if(! $this->quote_res_nums_checked[$a_quote_res_num]){ //�`�F�b�N���Ă��Ȃ��ԍ����ċA�`�F�b�N
						$this->quote_res_nums_checked[$a_quote_res_num] = true;
						
						$datalinear = $this->thread->explodeDatLine($this->thread->datlines[$a_quote_res_num-1]);
						$quote_name = $datalinear[0];
						$quote_msg = $this->thread->datlines[$a_quote_res_num-1];
						$quote_res_nums = array_merge( $quote_res_nums, $this->checkQuoteResNums($a_quote_res_num, $quote_name, $quote_msg) );
					 }
				 }
			 }
			//$name=preg_replace("/([0-9]+)/", "", $name, 1);
		}
	
		// >>1�̃����N����������O��
		// <a href="../test/read.cgi/accuse/1001506967/1" target="_blank">&gt;&gt;1</a>
		$msg=preg_replace("/<a href=\"\.\.\/test\/read\.cgi\/{$this->thread->bbs}\/{$this->thread->key}\/([-0-9]+)\" target=\"_blank\">&gt;&gt;([-0-9]+)<\/a>/","&gt;&gt;\\1", $msg);

		//echo $msg;
		if( preg_match_all("/(&gt;|��)?(&gt;|��)(([0-9- ,=&.]|�A)+)/", $msg, $out, PREG_PATTERN_ORDER) ){

			foreach($out[3] as $numberq){
				//echo $numberq;
				if( preg_match_all("/[0-9]+/", $numberq, $matches, PREG_PATTERN_ORDER) ){
				
					foreach($matches[0] as $a_quote_res_num){
					
						//echo $a_quote_res_num;
						
						if(!$a_quote_res_num){break;}
						$quote_res_nums[]=$a_quote_res_num;
				
						if($a_quote_res_num != $res_num){ //�������g�̔ԍ��Ɠ���łȂ���΁A
							if(! $this->quote_res_nums_checked[$a_quote_res_num]){ //�`�F�b�N���Ă��Ȃ��ԍ����ċA�`�F�b�N
								$this->quote_res_nums_checked[$a_quote_res_num] = true;
								
								$datalinear = $this->thread->explodeDatLine($this->thread->datlines[$a_quote_res_num-1]);
								$quote_name = $datalinear[0];
								$quote_msg = $this->thread->datlines[$a_quote_res_num-1];
								$quote_res_nums = array_merge( $quote_res_nums, $this->checkQuoteResNums($a_quote_res_num, $quote_name, $quote_msg) );
							 }
						 }
						 
					 }
					 
				}
				
			}
			
		}
		
		return $quote_res_nums;
	}
	
}
?>
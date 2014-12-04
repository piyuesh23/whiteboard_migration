<?php
/**
 * Theme pre-processor for client_profileview.
 */
function whiteboard_migration_preprocess_client_profileview(&$variables) {
  global $user;
  $node = $variables['node'];
  module_load_include("inc", "whiteboard_api", "enquiry");
  module_load_include("inc", "wb_translation", "wb_translation");
  module_load_include("inc", "whiteboard_api", "global");
  module_load_include("inc", "whiteboard_api", "solr");
  module_load_include("inc", "whiteboard_api", "job");
  drupal_add_css(drupal_get_path('module', 'enquiry_view') . '/css/enquiry.css');
  drupal_add_css(drupal_get_path('module', 'clientManagement_cm') . '/css/client_profileView.css');
  module_load_include('inc', 'content', 'content.admin');
  $groups = fieldgroup_groups('client_profile');
  $workerQ = "&fq=type:translation&fq=is_cck_field_translation_trans_worker:" . $user->uid . "&fq=is_cck_field_translation_id:" . $node->nid . "&fq=ss_cck_field_translation_status:UT";
  $worker = runSolrQuery($workerQ);
  if ($worker->response->numFound > 0) {
    $variables['editCp'] = l('Edit client profile', 'node/' . $node->nid . "/edit/" . md5('client_profile') . "/translator", array('attributes' => array('class' => 'automodal')));
  }

  $node = $variables['node'];
  $data = $transFields = array();
  foreach ($groups as $key => $value) {
    foreach ($value as $key1 => $value1) {
      if ($key1 == 'label') {
        $variables['blockEditableArray'][$key]['title'] = $value1;
      }
      if ($key1 == 'fields') {
        $tempArray = array();
        foreach ($value1 as $key2 => $value2) {
          foreach ($value2 as $key3 => $value3) {
            if ($key3 == 'label') {
              $tempViewArray = $node->$key2;

              if ($key2 == 'field_client_profile_address') {
                module_load_include('inc', 'addresses', 'addresses');
                $provinces = _addresses_province_get($tempViewArray[0]['country']);
                $tempViewArray[0]['view'] = str_replace("]", "", $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("[", "", $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("wb_client_billing_postal_code", $tempViewArray[0]['postal_code'], $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("wb_client_code", "[[wb_client_code]]", $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("wb_client_billing_province", $provinces[$tempViewArray[0]['province']], $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("wb_client_billing_city", $tempViewArray[0]['city'], $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("wb_client_billing_street", $tempViewArray[0]['street'], $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("wb_client_billing_org", $node->field_client_profile_org[0]['view'], $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("wb_client_billing_department", $node->field_client_profile_department[0]['view'], $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("field_pay_pref_lastname-formatted", $node->field_client_profile_lastname[0]['view'], $tempViewArray[0]['view']);
                $tempViewArray[0]['view'] = str_replace("field_cp_pay_pref_firstname-formatted", $node->field_client_profile_firstname[0]['view'], $tempViewArray[0]['view']);
                $tempArray[$value3] = whiteboardTokenReplace($node->nid, $tempViewArray[0]['view']);
              }
              else {
//                        $tempArray[$value3] = whiteboardTokenReplace($node->nid, $tempViewArray[0]['view']);
                $tempArray[$value3] = $tempViewArray[0]['number'] ? $tempViewArray[0]['number'] : whiteboardTokenReplace($node->nid, $tempViewArray[0]['view']);
              }
            }
          }
        }
        $variables['blockEditableArray'][$key]['fields'] = $tempArray;
      }
    }
  }

  $variables['editUrl'] = $node->nid . "/edit/" . md5('client_profile');
  foreach ($variables['blockEditableArray'] as $bIKey => $bI) {
    foreach ($bI["fields"] as $fldKey => $fld) {
      $variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['fieldname'] = db_result(db_query("SELECT i.field_name FROM {content_node_field_instance} i
WHERE i.label = '%s'
AND i.type_name = 'client_profile'", $fldKey));
    }
  }
}

/**
 * Theme preprocessor for job_questionsview.
 */
function whiteboard_migration_preprocess_job_questionsview(&$variables) {
  $node = $variables['node'];
  $job_nid = $node->field_job_questions_job_id[0]['nid'];
  $client = runSolrQuery("&fq=nid:$job_nid");
  $client_uid = $client->response->docs[0]->is_cck_field_enquiry_contact;
// print_r($client_uid);

//Check for Test Enquiry
  $variables['is_test_client'] = FALSE;
  $variables['check_test_client_query'] = db_fetch_object(db_query("select mail from {users} where uid = %d", $client_uid));
  $variables['result'] = db_query("SELECT * FROM {content_type_job_questions_details} c
join {node} as n on n.vid = c.vid  and n.status  = 1
where c.field_job_que_details_job_que_nid= '%d' ORDER BY field_job_que_details_process_value ASC ", $node->nid);
}

/**
 * Theme pre-processor for jobview.
 */
function whiteboard_migration_preprocess_jobview(&$variables) {
  module_load_include("inc", "whiteboard_api", "global");
  module_load_include("inc", "whiteboard_api", "solr");
  module_load_include("inc", "enquiry_view", "enquiry_view");
  module_load_include("inc", "job_view", "job_view");
  module_load_include("inc", "paymentsGlobal", "paymentsGlobal");
  module_load_include("inc", "enquiry_core", "enquiry_core");
  module_load_include('inc', 'kyc', 'kyc');
  /* * ***********Reviewer Auto-llocation chages************* */
  module_load_include("inc", "reviewerAllocation", "reviewerAllocation");
  /* * ***********Reviewer Auto-llocation chages************* */
  $node = $variables['node'];
  global $user;
  // Get the view of node for job and enquiry
  $variables['jobNode'] = getTheViewOfNode($node);
  $variables['enquiryNode'] = getTheViewOfNode($node->field_job_enquiry_id[0]['nid']);
// Get the solr node load for enquiry and job
  $variables['solrNode'] = solr_node_load($node->nid);
  $variables['solrEnqNode'] = solr_node_load($node->field_job_enquiry_id[0]['nid']);

  $client_uid = $variables['solrEnqNode']['node']->is_cck_field_enquiry_contact;
  $variables['is_test_client'] = FALSE;
  $variables['check_test_client_query'] = db_fetch_object(db_query("select mail from {users} where uid = %d", $client_uid));
  // Remove cover letter and LFTE for FREE MREs.
  $variables['type_of_enq'] = $variables['enquiryNode']->field_enquiry_type_of_enq[0]['value'];
  $variables['is_FREE_MRE'] = FALSE;
  if ($variables['type_of_enq'] == 'FMR') {
    $variables['is_FREE_MRE'] = TRUE;
  }

// get client code
  $variables['client_code_nid'] = getClientGroupNidFromClientProfileUserId($variables['enquiryNode']->field_enquiry_contact[0]['uid']);
  $variables['is_ELSVR'] = FALSE;
  if ($variables['client_code_nid'] == 5201413) {
    $variables['is_ELSVR'] = TRUE;
  }
  $variables['clientType'] = $variables['solrNode']['node']->ss_cck_field_enquiry_client_type_ind_cor == 'I' ? "IND" : "COR";
  $variables['lfteServiceNid'] = $variables['enquiryNode']->field_enquiry_service[0]['nid'];
  $variables['partnerId'] = $variables['solrNode']['node']->is_cck_field_enquiry_partner;
  $variables['sqlLfte'] = db_query("select * from {node} as n
                  JOIN {content_type_lfte} as l on n.vid=l.vid and l.field_lfte_client_type_value='" . $variables['clientType'] . "' and l.field_lfte_service_level_value='" . $variables['lfteServiceNid'] . "' and n.status=1
                  JOIN {content_field_lfte_applicable_partner} lap on lap.nid = n.nid and lap.field_lfte_applicable_partner_nid='" . $variables['partnerId'] . "'");

  $job_detail_result = db_query("select jd.nid from {content_type_job_details} jd
join {node} n1 on n1.vid = jd.vid where jd.field_job_details_job_id_nid = %d and n1.status = 1", arg(1));
  // Processes
  while ($return[] = db_fetch_object($job_detail_result)) {
    $jobDetailsNid = $return;
  }

  $variables['jobDetailsNid'] = $jobDetailsNid;
  $crossMarkImagePath = '/' . path_to_theme() . '/images/cross.png';
  $isJobSentToClient = ($variables['jobNode']->field_job_sent_to_client_date[0]['value']) ? TRUE : FALSE;
  if (is_array($jobDetailsNid) && !empty($jobDetailsNid)) {
    foreach ($jobDetailsNid as $key => $val) {
      if ($val->nid) {
        $jobDetails = node_load($val->nid, NULL, TRUE);
        $sl = $variables['enquiryNode']->field_enquiry_service[0]['nid'];
        $process = $jobDetails->field_job_details_process_id[0]["value"];
        $serviceLevelVid = explode("-", $jobDetails->field_job_details_process_id[0]["value"]);
//            $serviceLevel = db_result(db_query("SELECT field_servicelevel_process_value  FROM {content_field_servicelevel_process} where vid='%s' and delta = '%s'", trim($serviceLevelVid[0]), trim($serviceLevelVid[1])));
        $serviceLevel = db_fetch_object(db_query("SELECT n.nid, p.field_process_mapping_value mapping,
                p.field_process_internal_name_value intName,
                p.field_process_weight_value wgt,
                p.field_process_unit_type_value processUnitCountType
                FROM {content_type_processes} p
                JOIN {node} n ON p.vid = n.vid
                WHERE p.field_process_service_nid = %d
                AND p.field_process_mapping_value = '%s'", $sl, $process));

        $serviceLevels = node_load($variables['enquiryNode']->field_enquiry_service[0]["nid"]);
        $delta = explode("-", $process);
//            $name = $serviceLevel->intName ? $serviceLevel->intName : $serviceLevels->field_servicelevel_process[$delta[1]];
        $name = $serviceLevels->field_servicelevel_process[$delta[1]]['value'];
        $variables['blockEditableArray'][$name] ["fields"]["UploadedFiles"] = 0;
        $variables['blockEditableArray'][$name] = array(
          "job_nid" => $variables['jobNode']->nid,
          "job_title" => $variables['jobNode']->title,
          "job_detials_process" => $jobDetails->field_job_details_process_id[0]["value"],
          "title" => $name,
          "weight" => $serviceLevel->wgt,
          "processUnitCountType" => $serviceLevel->processUnitCountType
        );

        $variables['blockEditableArray'][$name]["fields"]["jobDetailsNid"] = $val->nid;
        $variables['blockEditableArray'][$name]["fields"]["RE_can_send_job:"] = FALSE;
        if ($user->uid == $jobDetails->field_job_details_user_id[0]["uid"]) {
          $userDetails = user_load($jobDetails->field_job_details_user_id[0]["uid"]);
          if (!$userDetails->fullName) {
            $userDetails = checkUserObjAndAddExtraData($userDetails);
          }
          $variables['blockEditableArray'][$name] ["fields"]["Allocated to:"] .= $userDetails->fullName . ", ";
          if ($jobDetails->field_job_details_mark_compl_by[0]['value'] == 'RE') {
            $eligibleRE = TRUE;
            $variables['blockEditableArray'][$name]["fields"]["RE_can_send_job:"] = TRUE;
          }
        }
        else {
          $autionResult = db_query("SELECT distinct  n2.nid  FROM {node}  as n
                JOIN {content_type_job_details} as jd on jd.field_job_details_job_id_nid = n.nid
                JOIN {node} as n1 on n1.type='job_details' and n1.vid = jd.vid
                JOIN {content_type_job_auction_details} as jad
                ON jad.field_job_au_job_details_id_nid = jd.nid
                JOIN {node} as n2 on n2.type='job_auction_details' and n2.vid = jad.vid
                JOIN {content_type_job} as j ON j.vid=n.vid
                where n.nid = %d and jd.field_job_details_process_id_value = '%s'
                AND n.type='job' and n2.status = 1 AND
                n1.status = 1", arg(1), $jobDetails->field_job_details_process_id[0]["value"]);

          $return = $auctionNidList = array();
          while ($return[] = db_fetch_object($autionResult)) {
            $auctionNidList = $return;
          }
          $variables['blockEditableArray'][$name] ["fields"]["Allocated to:"] = "";
          if ($auctionNidList) {
            foreach ($auctionNidList as $key => $au_val) {
              if ($au_val) {
                $auctionUserDetails = node_load($au_val->nid, NULL, TRUE);
                $userDetails = user_load($auctionUserDetails->field_job_au_user_id[0]["uid"]);
                $variables['blockEditableArray'][$name]["fields"]["showrePmFlag:"] = FALSE;
                if (in_array('InhouseETE', $userDetails->roles) && $auctionUserDetails->field_job_au_status[0]['value'] != 'R') {
                  $variables['blockEditableArray'][$name]["fields"]["RE_can_send_job:"] = FALSE;
                  $variables['blockEditableArray'][$name]["fields"]["showrePmFlag:"] = TRUE;
                }
                if ($jobDetails->field_job_details_mark_compl_by[0]['value'] == 'RE') {
                  $rePmFlag = TRUE;
                  $variables['blockEditableArray'][$name]["fields"]["RE_can_send_job:"] = TRUE;
                }
                $class = ($jobDetails->field_job_details_user_id[0]["uid"] != $userDetails->uid && $auctionUserDetails->field_job_au_status[0]["value"] == "R") ? "strike" : NULL;
                if (!$userDetails->fullName) {
                  $userDetails = checkUserObjAndAddExtraData($userDetails);
                }
                if ($class ? ($jobDetails->field_job_details_user_id[0]["uid"] == $userDetails->uid || count(array_intersect(array(
                        "PM",
                        "CM"
                      ), $user->roles))) : TRUE
                ) {
                  $variables['blockEditableArray'][$name] ["fields"]["Allocated to:"] .= "<span class=\"$class\">" . $userDetails->fullName . "</span>, ";
                }
              }
            }
          }
        }
        $variables['blockEditableArray'][$name] ["fields"]["allocated_user"] = $jobDetails->field_job_details_user_id[0]["uid"];
        $variables['blockEditableArray'][$name] ["fields"]["Expected start time:"] = getTheDateFormatNew($jobDetails->field_job_details_exp_start_time[0]["value"], "", $user, NULL, TRUE, NULL, NULL, NULL, NULL, TRUE);
        $variables['blockEditableArray'][$name] ["fields"]["Expected end time:"] = getTheDateFormatNew($jobDetails->field_job_details_exp_end_time[0]["value"], "", $user, NULL, TRUE, NULL, NULL, NULL, NULL, TRUE);
        $variables['blockEditableArray'][$name] ["fields"]["Actual start time:"] = $jobDetails->field_job_details_act_start_time[0]["value"] ? getTheDateFormatNew($jobDetails->field_job_details_act_start_time[0]["value"], "", $user, NULL, TRUE, NULL, NULL, NULL, NULL, TRUE) : NULL;
        $variables['blockEditableArray'][$name] ["fields"]["Actual end time:"] = $jobDetails->field_job_details_act_end_time[0]["value"] ? getTheDateFormatNew($jobDetails->field_job_details_act_end_time[0]["value"], "", $user, NULL, TRUE, NULL, NULL, NULL, NULL, TRUE) : NULL;

        if ($jobDetails->field_job_details_comment[0]["value"]) {
          $variables['blockEditableArray'][$name] ["fields"]["Comments:"] = $jobDetails->field_job_details_comment[0]["value"];
        }
        if ($jobDetails->field_job_details_process_instr[0]["value"]) {
          $variables['blockEditableArray'][$name] ["fields"]["Process instructions:"] = $jobDetails->field_job_details_process_instr[0]["value"];
        }
        if ($jobDetails->field_job_details_comment[0]["value"]) {
          $variables['blockEditableArray'][$name] ["fields"]["Log comments:"] = $jobDetails->field_job_details_comment[0]["value"];
        }
        // Added new process unit counts
        if ($jobDetails->field_job_details_processunit_c[0]["value"]) {
          $variables['blockEditableArray'][$name] ["fields"]["Process unit count:"] = $jobDetails->field_job_details_processunit_c[0]["value"];
        }

        $variables['blockEditableArray'][$name] ["fields"]["UploadedFiles"] = 0;
        if ($jobDetails->field_job_details_act_end_time[0]["value"]) {
          if (isset($jobDetails->field_job_details_files[0]['fid'])) {
            $variables['blockEditableArray'][$name] ["fields"]["UploadedFiles"] = count($jobDetails->field_job_details_files);
            foreach ($jobDetails->field_job_details_files as $fKey => $processFile) {
              $reMoveFileLink = '';
              if (in_array("PM", $user->roles) && !$isJobSentToClient && !in_array($variables['jobNode']->field_job_status[0]['value'], array(
                    'C',
                    'P'
                  ))
              ) {
                $reMoveFileLink = (!$isJobSentToClient) ? l("<img src='$crossMarkImagePath' title='Delete " . $processFile['filename'] . "' />", "job-process-remove-file/" . $jobDetails->nid . "/" . $processFile['fid'], array(
                    "attributes" => array("class" => "remove-single-file"),
                    'html' => array('html' => 'true')
                  )) : "";
              }
              else {
                if ((in_array("Freelancer", $user->roles) || in_array("InhouseETE", $user->roles)) && !in_array($variables['jobNode']->field_job_status[0]['value'], array(
                      'C',
                      'P'
                    )) && $jobDetails->field_job_details_user_id[0]["uid"] == $user->uid
                ) {
                  $reMoveFileLink = (!$isJobSentToClient) ? l("<img src='$crossMarkImagePath' title='Delete " . $processFile['filename'] . "' />", "job-process-remove-file/" . $jobDetails->nid . "/" . $processFile['fid'], array(
                      "attributes" => array("class" => "remove-single-file"),
                      'html' => array('html' => 'true')
                    )) : "";
                }
              }
              $variables['blockEditableArray'][$name] ["fields"]["$name complete files:"] .= "<div class='processFilesClass'>" . theme('filefield_file', $processFile) . " " . $reMoveFileLink . "</div>";
            }
            $variables['blockEditableArray'][$name] ["fields"]["Download files:"] = l("DOWNLOAD " . strtoupper($name) . " FILES", "inhouse-download-files/" . $jobDetails->nid, array("attributes" => array("class" => "process-zip-file-download")));
          }
        }
        $variables['blockEditableArray'][$name] ["fields"]["Allocated to:"] = trim($variables['blockEditableArray'][$name] ["fields"]["Allocated to:"], ", ");
        $variables['blockEditableArray'][$name] ["fields"]["uidOpacity"] = $userDetails->inhouse_user_details->uid ? $userDetails->inhouse_user_details->uid : $userDetails->profile->uid;

        /*             * ***********Reviewer Auto-llocation chages************* */
        $variables['blockEditableArray'][$name] ["fields"]["EstimatedReviewTime"] = getReviewTimeForFrontend('', $variables['jobNode']->nid, $variables['blockEditableArray'][$name]['title']);
        // $variables['blockEditableArray'][$name] ["fields"]["EstimatedReviewTime"] = 200;
        /*             * ***********Reviewer Auto-llocation chages************* */
      }
    }
  }
  if (is_array($variables['blockEditableArray'])) {
    foreach ($variables['blockEditableArray'] as $bIKey => $bI) {
      $variables['blockEditableArray'][$bIKey]['prfNid'] = db_result(db_query("select distinct(cp.nid)
                                    from {content_type_processes} cp
                                    join {node} n on n.vid = cp.vid
                                    join {content_field_process_reference_files} prf on prf.nid = cp.nid
                                    join {node} n1 on n1.vid = prf.vid
                                    where cp.field_process_mapping_value = '" . $bI['job_detials_process'] . "'
                                    and prf.field_process_reference_files_fid is not null"));
    }
  }
  $query = db_query("select u.uid, name from {users} u join {users_roles} ur on ur.uid = u.uid where ur.rid = 109 order by name asc");
  while ($result = db_fetch_array($query)) {
    $pmUid = $result['uid'];
    $variables['allPms'] .= "<li id='" . $pmUid . "'>" . $result['name'] . "</li>";
  }

  if ($user->uid == 1 || in_array('CM', $user->roles) || in_array('InhouseETE', $user->roles) || in_array('PM', $user->roles)) {
    $reviwerQuery = "select count(*) reCheck from
            {content_type_job_details} jd
            join {node} n on n.vid = jd.vid and n.status = '1'
            and field_job_details_job_id_nid = '" . $variables['jobNode']->nid . "' and field_job_details_user_id_uid = $user->uid
            and field_job_details_mark_compl_by_value = 'RE'";
    $variables['reviwerValue'] = db_result(db_query($reviwerQuery));
  }

  if (in_array('CM', $user->roles) || in_array('PM', $user->roles)) {
    $variables['count_cm_pm'] = db_result(db_query("select count(c.nid) from {comments} c
                        where nid=%d and c.subject = 'SA-autocomplete'", $variables['enquiryNode']->nid));
  }
}

/**
 * Theme pre-processor for enquiryview
 */
function whiteboard_migration_preprocess_enquiryview(&$variables) {
  module_load_include("inc", "whiteboard_api", "enquiry");
  module_load_include("inc", "whiteboard_api", "global");
  module_load_include("inc", "whiteboard_api", "comment");
  module_load_include("inc", "enquiry_view", "enquiry_view");
  module_load_include("inc", "wb_translation", "wb_translation");
  module_load_include('inc', 'SCPlusCore', 'SCPlusCore');
  module_load_include('inc', 'kyc', 'kyc');
  global $user;
  $node = $variables['node'];
  $variables['ec_id'] = db_fetch_object(db_query('select ee.field_enquiry_use_card_value, field_enquiry_editage_card_id_value from {content_type_enquiry_extended} ee join {node} n on n.vid=ee.vid where ee.field_enquiry_extended_enquiry_nid=%d', $variables['node']->nid));
  $variables['jobNodeNid'] = db_result(db_query("select n.nid from {node} n join {content_type_job} j on j.vid = n.vid and n.type='job' and j.field_job_enquiry_id_nid = %d", $node->nid));
  $variables['check_test_client_query'] = db_fetch_object(db_query("select mail from {users} where uid = %d", $node->field_enquiry_contact[0]['uid']));
  $details = getEnquiryNotEditableDetails($node->nid);
  $variables['enNid'] = db_result(db_query("SELECT cpt.nid FROM {content_type_client_profile_translation} cpt JOIN {node} n ON  n.vid = cpt.vid AND n.nid = cpt.nid WHERE n.language = '%s' AND cpt.field_client_profile_trans_nid_nid = %d", 'en', $details->clientProfileNid));
  $partnerDetails = node_load(array(
      "title" => $details->partner,
      "type" => "partner"
    ));
  if (!empty($partnerDetails->nid)) {
    $variables['partnerNetworkTitle'] = db_result(db_query("SELECT n.title FROM {content_field_partner_network_partner} p
join {node} n on n.nid = p.nid
where p.field_partner_network_partner_nid = " . $partnerDetails->nid . " limit 1"));
  }

  $details->ctranslationNid = $variables['enNid'];
  $clientProfileDetails = new stdClass();
  $clientProfile = new stdClass();
  $clientProfileDetails = getTheViewOfNode($details->clientProfileNid);
  if ($clientProfileDetails) {
    $cpQ = "&fq=type:client_profile&fq=nid:" . $details->clientProfileNid;
    $cp = runSolrQuery($cpQ);
    $salQ = "&fq=nid:" . $cp->response->docs[0]->is_cck_field_client_profile_salutation;
    $sal = runSolrQuery($salQ);
    $salPayQ = "&fq=nid:" . $cp->response->docs[0]->is_cck_field_cp_pay_pref_salutation;
    $salPay = runSolrQuery($salPayQ);

    $clientProfile->field_enquiry_client_code = $cp->response->docs[0]->is_cck_field_enquiry_client_code;
    $clientProfile->field_enquiry_client_code_text = $cp->response->docs[0]->is_cck_field_enquiry_client_code_text;
    $clientProfile->field_client_profile_salutation = $sal->response->docs[0]->title;
    $clientProfile->field_client_profile_firstname = $cp->response->docs[0]->ss_cck_field_client_profile_firstname;
    $clientProfile->field_client_profile_lastname = $cp->response->docs[0]->ss_cck_field_client_profile_lastname;
    $clientProfile->field_client_profile_native_name = $cp->response->docs[0]->ss_cck_field_client_profile_native_name;
    $clientProfile->field_client_profile_pri_email = $cp->response->docs[0]->ss_cck_field_client_profile_pri_email;
    $clientProfile->field_client_profile_alt_email = $cp->response->docs[0]->ss_cck_field_client_profile_alt_email;
    $clientProfile->field_client_profile_email_pref = ($cp->response->docs[0]->ss_cck_field_client_profile_email_pref == 'H') ? 'HTML' : 'Plain text';
    $clientProfile->field_client_profile_ref_source = $cp->response->docs[0]->ss_cck_field_client_profile_ref_source_text;
    $clientProfile->field_client_profile_ref_details = $cp->response->docs[0]->ss_cck_field_client_profile_ref_details;
    $clientProfile->field_client_profile_primary_ph = $cp->response->docs[0]->ss_cck_field_client_profile_primary_ph;
    $clientProfile->field_client_profile_address = $cp->response->docs[0]->ss_cck_field_client_profile_address;
    $clientProfile->field_client_profile_org = $cp->response->docs[0]->ss_cck_field_client_profile_org;
    $clientProfile->field_client_profile_job_title = $cp->response->docs[0]->ss_cck_field_client_profile_job_title;
    $clientProfile->field_client_profile_department = $cp->response->docs[0]->ss_cck_field_client_profile_department;

    $clientProfile->field_cp_pay_pref_salutation = $salPay->response->docs[0]->title;
    $clientProfile->field_cp_pay_pref_firstname = $cp->response->docs[0]->ss_cck_field_cp_pay_pref_firstname;
    $clientProfile->field_pay_pref_lastname = $cp->response->docs[0]->ss_cck_field_pay_pref_lastname;
    $clientProfile->field_pay_pref_organization = $cp->response->docs[0]->ss_cck_field_pay_pref_organization;
    $clientProfile->field_pay_pref_department = $cp->response->docs[0]->ss_cck_field_pay_pref_department;
    $clientProfile->field_pay_pref_payment_addr = $cp->response->docs[0]->ss_cck_field_pay_pref_payment_addr;
    $clientProfile->field_pay_pref_payment_mail = $cp->response->docs[0]->ss_cck_field_pay_pref_payment_mail;
    $clientProfile->field_client_profile_email_name = $cp->response->docs[0]->ss_cck_field_client_profile_email_name;
  }

  $variables['clientProfile'] = $clientProfile;
  $clientCodeForCoupons = $clientProfile->field_enquiry_client_code_text;
  $clientEmailForCoupons = $clientProfile->field_client_profile_pri_email;
  $clientDomainForCoupons = getScDomainName("http://" . $clientEmailForCoupons);

  $couponsQ = 'SELECT c.coupon_code
                FROM {coupons_tracker} c
                JOIN {content_type_campaign_master} cm on cm.nid = c.campaign_id
                WHERE c.status = 1
                AND c.coupon_status = "UN"
                AND cm.field_partner_nid = "' . $partnerDetails->nid . '"
                AND (c.client_code="' . $clientCodeForCoupons . '" OR c.domain = "' . $clientDomainForCoupons . '")';

  $variables['couponsR'] = db_query($couponsQ);

  if (in_array('CM', $user->roles) || in_array('PM', $user->roles)) {
    $variables['count_cm_pm'] = db_result(db_query("select count(c.nid) from {comments} c
                        where nid=%d and c.subject = 'SA-autocomplete'", $node->nid));
  }
}

/**
 * Theme pre-processor for invoiceview
 */
function whiteboard_migration_preprocess_invoiceview(&$variables) {
  module_load_include("inc", "whiteboard_api", "global");
  $node = $variables['node'];

  $query = "&q=&fq=type:payment_outstanding&fq=ss_po_common_entity_type:invoice&fq=is_cck_field_payment_outstanding_inv_dn:" . $node->nid;
  $owned = runSolrQuery($query);
  $result = "";

  if ($owned->response->docs[0] != NULL) {
    $result = $owned->response->docs[0];


    $arrjobnid = $result->im_po_invoice_jobs_array;
    $arrenqnid = $result->im_po_invoice_enquiries_array;
  }

  $newresult = $result;

  $locale = $result->ss_po_common_market_currency_locale;


  $strnids = implode("%20OR%20", $arrjobnid);
  $strenqnid = implode("%20OR%20", $arrenqnid);

  $jobquery = "&q=&fq=nid:(" . $strnids . ")";
  $enqquery = "&q=&fq=nid:(" . $strenqnid . ")";

  $jobresult = runSolrQuery($jobquery . "&rows=" . count($arrjobnid));
  global $enqresult;
  $enqresult = runSolrQuery($enqquery . "&rows=" . count($arrenqnid));


  foreach ($jobresult->response->docs as $key => $value) {
    $variables['rows'][$value->is_cck_field_job_enquiry_id] = array(
      $value->title,
      "Enquiry",
      "Service Level",
      $value->fs_cck_field_enquiry_unit_count,
      getTheCurrencyFromLocale($value->fs_cck_field_enquiry_rate, $locale, "rate"),
      getTheCurrencyFromLocale($value->fs_cck_field_enquiry_gross_price, $locale),
      getTheCurrencyFromLocale($value->fs_cck_field_enquiry_net_price, $locale),
      getTheCurrencyFromLocale(($value->fs_cck_field_enquiry_price_after_tax - $value->fs_cck_field_enquiry_net_price), $locale, "rate"),
      getTheCurrencyFromLocale($value->fs_cck_field_enquiry_price_after_tax, $locale),
      solrDisplayDate($value->created, 'jS F, Y'),
      ""
    );
  }

  //print_r( $owned->response->docs[0]);
  //$node->nid . $arrjobnid  . $jobquery);
  //die("Works");
  //echo "------------------------";

  global $user;
  $roles = $user->roles;
  $roles = array_flip($roles);

  foreach ($enqresult->response->docs as $key => $value11) {
    $variables['rows'][$value11->nid][1] = $value11->title;
    // $variables['rows'][$value11->nid][9] = getTheDateFormat($value11->created);
    $editPaymentInstructionlink = $value11->ss_cck_field_enquiry_payment_instr . " " . l("Edit", "edit-payment-instructions/" . $value11->nid . "/$node->nid", array("attributes" => array('target' => '_blank')));
    $paymentInstructionlink = (isset($value11->ss_cck_field_enquiry_payment_instr)) ? $value11->ss_cck_field_enquiry_payment_instr : '-';
    $finalLink = (!isset($roles['Vendor Printing'])) ? $editPaymentInstructionlink : $paymentInstructionlink;
    $variables['rows'][$value11->nid][10] = $finalLink;
    $variables['rows'][$value11->nid][2] = db_result(db_query("select title from {node} where nid =" . $value11->is_cck_field_enquiry_service));
    if ($enqresult->response->docs[0]->ss_cck_field_enquiry_client_type_ind_cor == "C") {
      $variables['rows'][$value11->nid][] = $value11->ss_cck_field_enquiry_spl_identifier ? $value11->ss_cck_field_enquiry_spl_identifier : "-";
    }
  }

  $variables['blockEditableArray'] = array(
    "invoiceDetails" => array(
      "title" => "INVOICE DETAILS",
      "fields" => array(
        "Invoice No:" => "$node->title",
        "Invoice Date:" => "$node->created",
        "Client Code:" => "Client Code",
        "Invoice Amount" => "Invoice Amount",
        "Invoice outstanding amount" => "Invoice outstanding amount",
        "Email status(Emailed by if any)" => "Email Status",
        "Email Sent date" => "Email Sent date",
        "Sent to Email id" => "Sent to Email id",
        "Print Status" => "$node->field_invoice_print_status",
        "Print date" => "Print date",
        "Last modified by" => "$node->revision_uid",
        "Last modified on" => "$node->changed"
      )
    ),
    "paymentDetails" => array(
      "title" => "PAYMENT DETAILS",
      "fields" => array("Invoice Type:" => "Invoice Type")
    )
  );

  foreach ($variables['blockEditableArray'] as $bIKey => $bI) {
    foreach ($bI["fields"] as $fldKey => $fld) {
      switch ($fldKey) {
        case "Print date":
          $variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['resultdate_printdate'] = db_result(db_query("select CreatedOn from {ps_printing_lots}  where lotid = (select m.lotid FROM  {ps_printing}  m where id=" . $node->field_invoice_print_status[0][value] . ")"));
          break;

        case "Email Sent date" :
          $variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['resultdate_email_sent_date'] = db_fetch_object(db_query("select m.created, m.sent FROM {messaging_store} m where m.mqid=" . $node->field_invoice_email_status[0][value]));
          break;

        case "Email status(Emailed by if any)":
          $resultesent = db_fetch_object(db_query("select m.created, m.sent FROM {messaging_store} m where m.mqid=" . $node->field_invoice_email_status[0][value]));
          if ($resultesent->sent != NULL && $resultesent->sent != 0) {
            $resultemailstatus = db_result(db_query("select name from {users} where uid = (select m.sender FROM  {messaging_store}  m where m.mqid=" . $node->field_invoice_email_status[0][value] . ")"));
            $variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['resultdate_email_status'] = "Sent (" . $resultemailstatus . ")";
          }
          else {
            $variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['resultdate_email_status'] = "Queued for sending";
          }
          break;

        case "Sent to Email id":
          $resultesent = db_fetch_object(db_query("select m.created, m.sent FROM {messaging_store} m where m.mqid=" . $node->field_invoice_email_status[0][value]));

          if ($resultesent->sent != NULL && $resultesent->sent != 0) {
            $resultemail = db_result(db_query("select name from {users} where uid = (select replace(m. destination, 'user:','') as uid  FROM  {messaging_store}  m where m.mqid=" . $node->field_invoice_email_status[0][value] . ")"));
            $variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['resultdate_email_sent_id'] = $resultemail;
          }
          else {
            $variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['resultdate_email_sent_id'] = "- None -";
          }
          break;

        case "Last modified by":
          $$variables['blockEditableArray'][$bIKey]['fields'][$fldKey]['last_modified_by'] = db_result(db_query("select name from {users} where uid =" . $fld));
          break;
      }
    }
  }
}

/**
 * Moved from invoiceview.tpl.php
 * @param $nid
 * @param $field
 * @param null $result
 * @param null $locale
 */
function invoiceViewSolr($nid, $field, $result = NULL, $locale = NULL) {
  global $user;

  module_load_include("inc", "whiteboard_api", "global");
  if ($result != NULL && $locale != NULL) {
    print_r(getTheCurrencyFromLocale($result->$field, $result->ss_po_common_market_currency_locale));
  }

  else {
    if ($result != NULL) {
      print_r($result->$field);
    }
    else {
      print_r("-- None --");
    }
  }
}

/**
 * Moved from invoiceview.tpl.php
 * @param $nid
 * @param $field
 * @param null $cp
 * @param null $result1
 */
function invoiceViewSolr2($nid, $field, $cp = NULL, $result1 = NULL) {


  module_load_include("inc", "whiteboard_api", "global");
  global $user;


  if ($field == "ss_po_common_pia_exploded" || $field == "ss_po_common_client_code_text") {
    //  $query = "&q=&fq=type:payment_outstanding&fq=is_cck_field_payment_outstanding_inv_dn:$nid";
    // $owned = runSolrQuery($query);

    $tempresult = unserialize($result1->$field);
    //print_r($owned->response->docs[0]->$field);
    $i = 0;

    if ($tempresult) {
      foreach ($tempresult as $key => $value) {
        $i++;
        $payadjdisplay = array();


        $paymode = "";
        foreach ($value as $key1 => $value1) {


          if ($key1 == "field_pia_payment_id_nid") {
            $query1 = "&q=&fq=is_cck_field_payment_outstanding_inv_dn:$value1";
            $owned1 = runSolrQuery($query1);

            if (!$payadjdisplay[$i]) {
              $payadjdisplay[$i] = l(ucfirst($owned1->response->docs[0]->ss_po_common_entity_type) . " " . $i . ":", "node/" . $value['field_pia_payment_id_nid'], array("attributes" => array('target' => '_blank')));
            }


            $paymode = $owned1->response->docs[0]->ss_po_payment_mode_name_text;
            $paydate = $owned1->response->docs[0]->tds_po_common_date_to_consider;

            $query1 = "&q=&fq=type:markets_master&fl=ss_cck_field_market_currency_locale&fq=ss_cck_field_market_locale_iso_code:" . $owned1->response->docs[0]->ss_po_payment_source_currency_locale;
            $owned1 = runSolrQuery($query1);
            $paymentlocale = $owned1->response->docs[0]->ss_cck_field_market_currency_locale;
          }

          if ($key1 == "field_pia_matched_amount_value") {

            $amt = $value1;
            $payadjdisplay[$i] .= getTheCurrencyFromLocale($amt, $paymentlocale) . " (" . solrDisplayDate($paydate, 'jS F, Y') . " / " . $paymode . ")<br />\n";
          }

          if ($key1 == "ss_po_common_client_code_text") {
            $payadjdisplay[$i] .= $owned1->response->docs[0]->ss_po_common_client_code_text;
          }
        }
        foreach ($payadjdisplay as $value) {
          print($value);
        }
      }
    }
  }

  if ($field == "ss_po_common_adjustments_exploded") {
    // $query = "&q=&fq=type:payment_outstanding&fq=is_cck_field_payment_outstanding_inv_dn:$nid";
    //  $owned = runSolrQuery($query);

    $tempresult = unserialize($result1->$field);
    //   print_r($owned->response->docs[0]->$field);


    $i = 0;
    if ($tempresult) {
      foreach ($tempresult as $key => $value) {
        $i++;
        print_r("Adjustment $i :");

        $reason = "";
        foreach ($value as $key1 => $value1) {

          if ($key1 == "field_payment_adj_reason_value") {

            $reason = $value1;
            $payadjcreated = db_result(db_query("select created from {node} where nid=" . $value["field_payment_adj_payment_id_nid"]));


            $amt1 = $value["field_payment_adj_amount_value"];
            $sign = $value["field_payment_adj_sign_value"];
            print_r(getTheCurrencyFromLocale($amt1, $result1->ss_po_common_market_currency_locale) . " (" . $sign . ") (" . format_date($payadjcreated, 'custom', 'jS F, Y') . "/" . $reason . ")");
            //  print_r ($owned1->response->docs[0]->field_payment_adj_reason_value); // . " (" . $paydate . " / " . $paymode . ")<br />\n");
          }
        }
      }
    }

    if ($i == 0) {
      print_r("- None -");
    }
  }

  if ($field == "im_po_invoice_enquiries_array") {


    $contactUid = $result1->is_po_common_cg_admin_uid;
    $contactNid = db_result(db_query("select * from {node} where uid = '$contactUid' and type = 'client_profile' "));
    if ($result1->ss_po_invoice_invoice_frequency == 'J') {


      global $enqresult;


      if ($enqresult->response->docs[0]->is_cck_field_enquiry_invoice_level_q) {
        module_load_include("inc", "enquiry_view", "enquiry_view");
        module_load_include("inc", "whiteboard_api", "enquiry");
        $enquiryInvoiceNode = node_load($enqresult->response->docs[0]->is_cck_field_enquiry_invoice_level_q);
        $enquiryInvoiceType = explode('_', $enquiryInvoiceNode->type);
        $quesParams['quesNode'] = $enquiryInvoiceNode;
        $quesParams['fieldNameInitial'] = "field_{$enquiryInvoiceType[2]}_";
        $quesParams['counter'] = $counter + 1;
        $viewInvoiceQuestionsArr = getViewOfCustomQuestions($quesParams);
        $viewInvoiceQuestions = $viewInvoiceQuestionsArr['viewOfQuestions'];
        $arrayOfVal[0]['view'] = strip_tags($viewInvoiceQuestions, '<div>');


        if ($field == "im_po_invoice_enquiries_array" && $cp == NULL) {

          print_r($arrayOfVal[0]['view']);
          return;
        }
      }
    }
    $enNid = db_result(db_query("SELECT cpt.nid FROM {content_type_client_profile_translation} cpt JOIN {node} n ON  n.vid = cpt.vid AND n.nid = cpt.nid WHERE n.language = '%s' AND cpt.field_client_profile_trans_nid_nid = %d", 'en', $contactNid));


    $clientProfileDetails = new stdClass();
    if (!empty($enNid)) {
      $clientProfileDetails = getTheViewOfNode($enNid);
    }
    else {
      $clientProfileDetails = getTheViewOfNode($contactNid);
    }


    switch ($cp) {
      case 1:

        // print_r($invresult->ss_po_common_cg_admin_cp_firstname);
        //  if (!empty($clientProfileDetails->field_cp_pay_pref_salutation[0][view]))

        print_r($clientProfileDetails->field_cp_pay_pref_salutation[0][view]);

        break;


      case 2:
        print_r($clientProfileDetails->field_cp_pay_pref_firstname[0][view]);
        break;

      case 3:
        print_r($clientProfileDetails->field_pay_pref_lastname[0][view]);
        break;

      case 4:
        print_r($clientProfileDetails->field_pay_pref_organization[0][view]);
        break;

      case 5:
        print_r($clientProfileDetails->field_pay_pref_department[0][view]);
        break;

      case 6:

        print_r(formatAddress($clientProfileDetails->field_pay_pref_payment_addr[0][view], $clientProfileDetails));
        break;

      case 7:
        print_r($clientProfileDetails->field_pay_pref_payment_mail[0][view]);
        break;
    }
  }
}


/**
 * Theme pre-processor for ECFFirstScreenRender
 */
function whiteboard_migration_preprocess_ECFFirstScreenRender(&$variables) {
  global $user;
  $data = $variables['data'];
  if ($variables['data']['enq_nid'] != NULL) {
    module_load_include("inc", "ECF_ALL", "ECF_ALL");
    $variables['enq_node'] = db_fetch_object(db_query("select e.* from {content_type_enquiry} e join {node} n on n.vid=e.vid where n.nid = %d", $data['enq_nid']));

    $variables['is_ses_pes'] = $data['is_ses_pes'];
  }
  $variables['selectedSAValue'] = "";
  $variables['selectedSA2Value'] = "";
  if ($variables['is_ses_pes'] == "yes") {
    $variables['selectedSAValue'] = $variables['enq_node']->field_enquiry_subject_area_2_nid;
    $variables['selectedSA2Value'] = $variables['enq_node']->field_enquiry_subject_area_2_nid;
    $variables['subjectArea1'] = db_result(db_query("select field_subject_area_1_nid from {content_type_subjectarea2} sa2 join {node} n on n.vid=sa2.vid where n.nid = %d", $variables['selectedSAValue']));
    $variables['subjectArea1Title'] = db_result(db_query("select title from {node} where nid=%d", $variables['subjectArea1']));
    $variables['saValue'] = ($variables['subjectArea1'] && $variables['selectedSA2Value']) ? $variables['subjectArea1'].'-'.$variables['selectedSA2Value'] : '';
    $variables['used_ec'] = db_result(db_query("select field_enquiry_use_card_value from {content_type_enquiry_extended} ee join {node} n on n.vid=ee.vid where field_enquiry_extended_enquiry_nid=%d",$variables['enq_node']->nid));
  }
  module_load_include("inc", "editageCards", "editageCards");
  $variables['ec_id'] = getEditageCardIDfromClientCodeNid($user->userDetails->client_group->nid);
  $variables['ec_name'] = db_result(db_query("select editage_card_name from {editageCard} where editage_card_id=%d",$variables['ec_id']));
}

/**
 * Theme pre-processor for ECFFirstCORScreenRender
 */
function whiteboard_migration_preprocess_ECFFirstCORScreenRender(&$variables) {
  $data = $variables['data'];
  if ($variables['data']['enq_nid'] != NULL) {
    module_load_include("inc", "ECF_ALL", "ECF_ALL");
    $variables['enq_node'] = db_fetch_object(db_query("select e.* from {content_type_enquiry} e join {node} n on n.vid=e.vid where n.nid = %d", $data['enq_nid']));

    $variables['is_ses_pes'] = $data['is_ses_pes'];
  }
  $variables['selectedSAValue'] = "";
  $variables['selectedSA2Value'] = "";
  if ($variables['is_ses_pes'] == "yes") {
    $variables['selectedSAValue'] = $variables['enq_node']->field_enquiry_subject_area_2_nid;
    $variables['selectedSA2Value'] = $variables['enq_node']->field_enquiry_subject_area_2_nid;
    $variables['subjectArea1'] = db_result(db_query("select field_subject_area_1_nid from {content_type_subjectarea2} sa2 join {node} n on n.vid=sa2.vid where n.nid = %d", $variables['selectedSAValue']));
    $variables['subjectArea1Title'] = db_result(db_query("select title from {node} where nid=%d", $variables['subjectArea1']));
    $variables['saValue'] = ($variables['subjectArea1'] && $variables['selectedSA2Value']) ? $variables['subjectArea1'].'-'.$variables['selectedSA2Value'] : '';
  }
}

/**
 * Theme pre-processor for issue_burnercore.
 */
function whiteboard_migration_preprocess_issue_burnercore(&$variables) {
  $node = $variables['node'];
  $variables['translatedData_fallback'] = unserialize(db_result(db_query("SELECT td.field_translated_fields_value FROM {content_type_translated_data} td
                JOIN {node} n  ON n.vid = td.vid
                WHERE td.field_translated_node_nid = %d", $node->nid)));
}

/**
 * Theme pre-processor for workshop_webinar.
 */
function whiteboard_migration_preprocess_workshop_webinar(&$variables) {
  $event = $variables['data']['event'];
  $partner_id = $event->field_ww_partner[0]["nid"];
  $variables['event_country_fallback'] = db_result(db_query("select field_partner_default_country_country from {content_type_partner} p
                                join {node} n on p.vid=n.vid where n.nid=%d", $partner_id));
}

/**
 * Theme pre-processor for paymentsmatchingview
 */
function whiteboard_migration_preprocess_paymentsmatchingview(&$variables) {
  $node = $variables['node'];
  if ($node->type == 'payment') {
    $paymentNode = $node;
    $variables['suspensePaymentNid'] = db_result(db_query("SELECT nid FROM {content_field_sp_payment_pref} where field_sp_payment_pref_nid = $paymentNode->nid limit 1"));
  }
}

/**
 * @param $variables
 */
function whiteboard_migration_preprocess_custompage_wikidetails(&$variables) {
  $variables['bigCats'] = array();
  $variables['bigCatTids'] = "";
  $rs = db_query("SELECT tid,name,description FROM {term_data} where tid = ".arg(1));
  while($row = db_fetch_object($rs)) {
    $variables['bigCats'][$row->tid]['name'] = $row->name;
    $variables['bigCats'][$row->tid]['tid'] = $row->tid;
    $variables['bigCats'][$row->tid]['description'] = $row->description;
    $variables['bigCatTids'] .= $row->tid . ",";
  }
  foreach($variables['bigCats'] as $key => $bigCat) {
    $rs = db_query("select sum(word_count) wc, sum(reliable_word_count) rwc,count(*) cnt from {node} n join {wiki_article} wa on n.vid=wa.vid where n.nid in (select nid from {term_node} tn where tn.tid = " . $bigCat['tid'] . ")");
    $variables['bigCats'][$key] = db_fetch_object($rs);
  }

  $user = $variables['user'];
  $variables['totalUnreadComments'] = db_result(db_query("select count(*) from {node} n
			INNER JOIN {comments} c ON n.nid = c.nid
			JOIN {history} h ON (h.nid = n.nid and c.timestamp > h.timestamp)
			where h.uid= '%d' and n.nid in (select nid from {term_node} tn where tn.tid = '%d' )", $user->uid, arg(1)));

  $variables['totalComments'] =db_result(db_query("select sum(comment_count) from {node} n
			INNER JOIN {node_comment_statistics}  c ON n.nid = c.nid
			where n.nid in (select nid from {term_node} tn where tn.tid = '%d' )",arg(1)));
}

/**
 * @param $variables
 */
function whiteboard_migration_preprocess_custompage_wikihome(&$variables) {
  $variables['bigCats'] = array();
  $variables['bigCatTids'] = "";
  $rs = db_query("SELECT tid,name,description FROM {term_data} where vid = 1");
  while($row = db_fetch_object($rs)) {
    $variables['bigCats'][$row->tid]['name'] = $row->name;
    $variables['bigCats'][$row->tid]['tid'] = $row->tid;
    $variables['bigCats'][$row->tid]['description'] = $row->description;
    $variables['bigCatTids'] .= $row->tid . ",";
  }

  foreach($variables['bigCats'] as $key => $bigCat) {
    $rs = db_query("select sum(word_count) wc, sum(reliable_word_count) rwc,count(*) cnt from node n join wiki_article wa on n.vid=wa.vid where n.nid in (select nid from {term_node} tn where tn.tid = " . $bigCat['tid'] . ")");
    $variables['bigCats'][$key]['row'] = db_fetch_object($rs);
  }
}

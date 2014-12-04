<?php

/**
 * @file
 *   Example drush command.
 *
 *   To run this *fun* command, execute `sudo drush --include=./examples mmas`
 *   from within your drush directory.
 *
 *   See `drush topic docs-commands` for more information about command authoring.
 *
 *   You can copy this file to any of the following
 *     1. A .drush folder in your HOME folder.
 *     2. Anywhere in a folder tree below an active module on your site.
 *     3. /usr/share/drush/commands (configurable)
 *     4. In an arbitrary folder specified with the --include option.
 *     5. Drupal's sites/all/drush folder.
 */

/**
 * Implementation of hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * See `drush topic docs-commands` for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function whiteboard_migration_files_drush_command() {
  $items = array();

  $items['migrate-whiteboard-enquiry-files'] = array(
    'description' => "Moves the completed job related files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwf --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwf --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwef'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-reference-files'] = array(
    'description' => "Moves the completed job related reference files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwf --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwf --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwrf'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-processed-files'] = array(
    'description' => "Moves the completed job related processed files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwf --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwf --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwpf'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-files-sent-to-c'] = array(
    'description' => "Moves the completed job related files sent to client to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwfsc --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwfsc --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwfsc'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-extra-files-sent-to-c'] = array(
    'description' => "Moves the completed job's extra files sent to client to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwefsc --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwefsc --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwefsc'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-job-question-files'] = array(
    'description' => "Moves the completed job's question files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwjqf --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwjqf --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwjqf'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-job-question-extra-files'] = array(
    'description' => "Moves the completed job's question extra files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwjqef --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwjqef --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwjqef'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-job-question-reference-files'] = array(
    'description' => "Moves the completed job questions's reference files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwjqrf --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwjqrf --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwjqrf'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-job-question-sent-files'] = array(
    'description' => "Moves the completed job question's sent files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwjqsf --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwjqsf --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwjqsf'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-job-question-upload-files'] = array(
    'description' => "Moves the completed job question's uploaded files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwjquf --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwjquf --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwjquf'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-job-feedback-files'] = array(
    'description' => "Moves the completed job's feedback files to s3 & saves the s3 url.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Limit the number of files to be migrated.',
        'example-value' => '10',
      ),
      'fid' => array(
        'description' => 'Moves only the file specified as an argument',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwjff --limit=10' => 'Move first 10 files that match the criteria to s3.',
      'drush mwjff --fid=10' => 'Moves file with fid 10 without checking if the job is actually processed or not.',
    ),
    'aliases' => array('mwjff'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  return $items;
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_enquiry_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_fetch_job_enquiry_fids';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_reference_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_fetch_job_enquiry_reference_fids';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_processed_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_fetch_job_enquiry_processed_fids';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}
/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_migrate_whiteboard_files_sent_to_c() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_fetch_job_files_to_c_fids';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_extra_files_sent_to_c() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_fetch_job_extra_files_to_c_fids';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_job_question_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_fetch_job_question_fids';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_job_question_extra_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_job_quesiton_extra_files';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_job_question_reference_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_job_quesiton_reference_files';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_job_question_sent_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_job_quesiton_sent_files';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_job_question_upload_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_job_question_upload_files';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_files_migrate_whiteboard_job_feedback_files() {
  $fid = drush_get_option('fid');
  $function_fetch_fids = '_whiteboard_migration_files_job_feedback_files';
  $limit = drush_get_option('limit');
  whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit, $fid);
}

/**
 * Helper function to prepare baches for files.
 */
function whiteboard_migration_files_prepare_batch($function_fetch_fids, $limit = NULL, $fid = NULL) {
  $fids = $function_fetch_fids($limit);
  print ('Total count: ' . count($fids));
  // if ($fid) {
  //   _whitebord_migration_files_process($fid);
  // }
  // else {
  //   $fids = $function_fetch_fids($limit);
  //   drush_print('Migrating ' . count($fids) . ' files.');
  //   whiteboard_migration_create_batch($fids, 1000,
  //     'whiteboard_migration_batch_process_files_create',
  //     'whiteboard_migration_invoice_batch_complete',
  //     'Batch Process for moving completed job files to s3.'
  //   );
  // }
}

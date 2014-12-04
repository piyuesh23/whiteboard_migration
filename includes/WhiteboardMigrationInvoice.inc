<?php
/**
 * @file
 */
require_once(drupal_get_path('module', 'whiteboard_migration') . '/aws-sdk-php/vendor/autoload.php');

use Aws\S3\S3Client;
use Aws\S3\Exception\S3Exception;

/**
 * Class WhiteboardMigrationInvoice
 */
Class WhiteboardMigrationInvoice {
  protected $client;
  protected $filepath;
  protected $bucket;

  public function __construct($filepath_online, $filepath_print, $nid, $vid, $type, $profile = 'default') {
    $this->client = S3Client::factory(array(
      'profile' => 'default'
    ));
    $this->filepath_online = $filepath_online;
    $this->filepath_print = $filepath_print;
    $this->nid = $nid;
    $this->vid = $vid;
    $this->type = $type;
  }

  public function migrateFile() {
    $s3_url_online = '';
    $s3_url_print = '';
    try {
      if ($this->filepath_online) {
        $resource = fopen(DRUPAL_BASEPATH . '/' . $this->filepath_online, 'r');
        $response = $this->client->upload('whiteboarddev', basename($this->filepath_online), $resource, 'public-read');
        $s3_url_online = $response['ObjectURL'];
      }

      if ($this->filepath_print) {
        $resource = fopen(DRUPAL_BASEPATH . '/' . $this->filepath_print, 'r');
        $response = $this->client->upload('whiteboarddev', basename($this->filepath_print), $resource, 'public-read');
        $s3_url_print = $response['ObjectURL'];
      }

      $this->updateInvoiceRecord($s3_url_online, $s3_url_print);
    } catch (S3Exception $e) {
      echo "There was an error uploading the file.\n";
    }
  }

  public function updateInvoiceRecord($s3_url_online, $s3_url_print) {
    $query = "Update whiteboard_migration_map set s3url_online = '%s',";
    $query .= " s3url_print = '%s', uploaded = 1 WHERE nid = %d AND vid = %d AND type = '%s'";
    db_query($query, array($s3_url_online, $s3_url_print, $this->nid, $this->vid, $this->type));

    if ($this->type == 'invoice') {
      whiteboard_migration_clear_archived_invoices($this->nid, $this->vid, $s3_url_online, $s3_url_print);
    }
    elseif ($this->type == 'payment') {
      whiteboard_migration_clear_archived_payments($this->nid, $this->vid, $s3_url_online, $s3_url_print);
    }
  }

  public function moveJobFiletos3() {
    try {
      $resource = fopen(DRUPAL_BASEPATH . '/' . $this->filepath, 'r');
      $response = $this->client->upload('whiteboarddev', $this->filepath, $resource, 'public-read');
      $query = "Update {files} SET filepath = '%s' WHERE filepath = '%s'";
      if ($response['ObjectURL']) {
        db_query($query, array($response['ObjectURL'], $this->filepath));
      }
    } catch (S3Exception $e) {
      echo "There was an error uploading the file.\n";
    }
  }
}

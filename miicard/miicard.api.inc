<?php

/**
 * @file
 * Classes to implement the miiCard API
 */

/**
 * Exception handling class.
 */
class miiCardException extends Exception {}


/**
 * miiCard OAuth and JSON API implementation class
 */
class miiCard {
  public $url;
  public $username;
  public $uid = 0;
  public $data;
  public $error = FALSE;
  public $expired = FALSE;
  public $token;
  
  protected $signature_method;
  protected $consumer;
  
  public function __construct($consumer_key, $consumer_secret, $oauth_token = NULL, $oauth_token_secret = NULL) {
    $this->signature_method = new OAuthSignatureMethod_HMAC_SHA1();
    $this->consumer = new OAuthConsumer($consumer_key, $consumer_secret);
    if (!empty($oauth_token) && !empty($oauth_token_secret)) {
      $this->token = new OAuthToken($oauth_token, $oauth_token_secret);
    }
  }

  protected function oauth_endpoint() {
    return variable_get('miicard_oauth', MIICARD_OAUTH);
  }
  protected function drupal_callback() {
    return url('user/login/miicard/callback', array('absolute' => TRUE));
  }
  protected function api_endpoint() {
    return variable_get('miicard_api', MIICARD_API);
  }
  
  public function token_store() {
    return array(
      'oauth_token' => $this->token->key,
      'oauth_token_secret' => $this->token->secret,
    );
  }
  
  /**
   * Perform a request
   *
   * @throws miiCardException
   */
  protected function request($url, $params = array(), $method = 'GET') {
    $data = '';
    if (count($params) > 0) {
      if ($method == 'GET') {
        $url = url($url, array('query' => $params));
      }
      else {
        $data = http_build_query($params);
      }
    }

    $headers = array();

    $response = drupal_http_request($url, array('headers' => $headers, 'method' => $method, 'data' => $data));
    if (!isset($response->error)) {
      return $response->data;
    }
    else {
      $error = $response->error;
      throw new miiCardException($error);
    }
  }
 
  public function get_request_token() {
    $url = $this->oauth_endpoint();
    $params = array('oauth_callback' => $this->drupal_callback());

    try {
      $response = $this->auth_request($url, $params);
      parse_str($response, $token);
      if (!array_key_exists('oauth_token', $token)) {
        throw new miiCardException("No token received from OAuth service - check credentials");
      }
      $this->token = new OAuthConsumer($token['oauth_token'], $token['oauth_token_secret']);
    }
    catch (Exception $e) {
      $token = array();
      $this->token = new OAuthConsumer('', '');
      $this->error = TRUE;
      watchdog("miiCard", "miiCard OAuth get request token failed: %error", array("%error" => $e->getMessage()), WATCHDOG_ERROR);
      drupal_set_message(t("Sorry, the miiCard service is currently unavailable. Please try again later"), 'error');
    }

    return $token;
  }

  public function get_authorise_url($token) {
    if (empty($token)) {
      $url = '';
    }
    else {
      $params = array('oauth_token' => $token['oauth_token']);
      $url = url($this->oauth_endpoint(), array('query' => $params));
    }
    
    return $url;
  }

  public function get_access_token($verifier) {
    $url = $this->oauth_endpoint();
    $params = array('oauth_verifier' => $verifier);
    
    try {
      $response = $this->auth_request($url, $params);
      if (empty($response)) {
        throw new miiCardException(t('Nothing received from miiCard'));
      }
      parse_str($response, $token);
      $this->token = new OAuthConsumer($token['oauth_token'], $token['oauth_token_secret']);
    }
    catch (Exception $e) {
      $token = array();
      $this->token = new OAuthConsumer('', '');
      $this->error = TRUE;

      watchdog("miiCard", "miiCard OAuth get access token failed: %error", array("%error" => $e->getMessage()), WATCHDOG_ERROR);
      drupal_set_message(t("Sorry, we could not authenticate you against the miiCard service. Please try again later"), 'error');
    }

    return $token;
  }

  public function auth_request($url, $params = array(), $method = 'GET') {
    $request = OAuthRequest::from_consumer_and_token($this->consumer, $this->token, $method, $url, $params);
    $request->sign_request($this->signature_method, $this->consumer, $this->token);

    switch ($method) {
      case 'GET':
        return $this->request($request->to_url());
      case 'POST':
        return $this->request($request->get_normalized_http_url(), $request->get_parameters(), 'POST');
    }
  }

  public function dictionary_request($quiet = FALSE) {
    if ($this->error) {
      return;
    }
    $data = array();
    
    $request = OAuthRequest::from_consumer_and_token($this->consumer, $this->token, 'POST', $this->api_endpoint());
    $request->sign_request($this->signature_method, $this->consumer, $this->token);
    $headers = array($request->to_header(), 'Content-Type: application/json');

    $curl_options = array(
      CURLOPT_URL => $this->api_endpoint(),
      CURLOPT_USERAGENT => 'miiCard Drupal module',
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_POST => TRUE,
      CURLOPT_HTTPHEADER => $headers,
      // TODO: Remove after beta testing!
      CURLOPT_SSL_VERIFYPEER => FALSE,
      CURLOPT_POSTFIELDS => '',
    );

    try {
      $ch = curl_init();
      curl_setopt_array($ch, $curl_options);
      $response = curl_exec($ch);
  
      $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
      $error = curl_error($ch);
      curl_close($ch);
      
      if ($response === FALSE) {
        throw new miiCardException($error);
      }
      elseif ($http_code != 200) {
        $this->expired = TRUE;
        throw new miiCardException(t('Nothing received from miiCard'));
      }

      $return = json_decode($response, TRUE);
      $data = $return['Data'];
      if ($data == NULL || !is_array($data)) {
        $this->expired = TRUE;
        throw new miiCardException(t("Invalid response from miiCard"));
      }
      
      if (preg_match('#/Date\((-?\d+)\d\d\d\)/#i', $data['LastVerified'], $matches)) {
        $data['LastVerified'] = intval($matches[1]);
      }
      else {
        $data['LastVerified'] = 0;
      }
            
      if (isset($data['PublicProfile'])) {
        if (preg_match('#/Date\((-?\d+)\d\d\d\)/#i', $data['PublicProfile']['LastVerified'], $matches)) {
          $data['PublicProfile']['LastVerified'] = intval($matches[1]);
        }
        else {
          $data['PublicProfile']['LastVerified'] = 0;
        }
      }

      $this->data = $data;
      $this->url = $data['ProfileUrl'];
      $this->username = $data['Username'];
    }
    catch (Exception $e) {
      $this->error = !$this->expired;
      
      if (!$quiet) {
        watchdog("miiCard", "miiCard JSON API call failed: %error", array("%error" => $e->getMessage()), WATCHDOG_ERROR);
        drupal_set_message(t("Sorry, we could not get your details from the miiCard service. Please try again later"), 'error');      
      }
      return FALSE;
    }

    return TRUE;  
  }
  
}

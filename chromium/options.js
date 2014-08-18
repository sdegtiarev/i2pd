function save() {
  var addr=$('#addr').val();
  var port=$('#port').val();
  localStorage.setItem('addr', addr);
  localStorage.setItem('port', port);
  alert('I2P Gateway at: '+addr+':'+port);
}

function alert(msg, type) {
  var $alert = $('#alert');
  var timeout;
  type = type || 'success'
  $alert.find('span.msg').html(msg);
  $alert.attr('class', 'alert fade in alert-'+type);
  $alert.show();
  clearTimeout(timeout);
  timeout=setTimeout(function() { $alert.slideUp(); }, 3000);
}


$(document).ready(function(){
  $('#save').on('click', save);
  $('#alert').alert();
  document.getElementById("addr").value=localStorage.getItem('addr');
  document.getElementById("port").value=localStorage.getItem('port');
});

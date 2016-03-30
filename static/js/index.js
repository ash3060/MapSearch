var count = 0;
var marker1 = new google.maps.Marker();
var marker2 = new google.maps.Marker();

function initialize() {
  count = 0;
  var mapOptions = {
    zoom: 13,
    center: new google.maps.LatLng(1.272443, 103.81064)
  };
  var map = new google.maps.Map(document.getElementById('map-canvas'),
      mapOptions);

  google.maps.event.addListener(map, 'click', function(e) {
    placeMarker(e.latLng, map);
    document.getElementById("bound"+count).value = e.latLng.toUrlValue();
    if (count == 2){
        count = 0;
    }
  });
}

function placeMarker(position, map) {
    count = count + 1;
    if (count == 1) {
        marker1.setMap(map);
        marker1.setPosition(position);
    }
    else if (count == 2) {
        marker2.setMap(map);
        marker2.setPosition(position);
    }
    //map.panTo(position);
}

google.maps.event.addDomListener(window, 'load', initialize);

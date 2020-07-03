var EmberConfig = (function(){
    var params = new URL(location).searchParams;
    return {
        display: {
            width: 1280,
            height: 720
        },
        server: {
            hubservice_url: params.get("hostname") || ("https://" + window.location.hostname + "/flan-hub/api")
        }
    };
})();

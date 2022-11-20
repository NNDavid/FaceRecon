<!DOCTYPE html>
<head>
<style>
#display-image{
  width: 400px;
  height: 225px;
  border: 1px solid black;
  background-position: center;
  background-size: cover;
}

</style>
</head>
<html>
<body>
<form method="GET" action="/submit">
    NAME: </br>
    <input type="text" name="name" value="" id="name" placeholder="Give your name" /></br>
    EMAIL: </br>
    <input type="text" name="email" placeholder="Give your email" /></br>
    YOUR IMAGE: </br>
    <input type="file" name="img" id="img" placeholder="Give your image" /></br>
    <button name="submit" value="Submit" onClick="formSub();"> 
    Submit
    </button>
  </form>
    </br>
    <div id="display-image"></div>
<script>
    
    var image_source = "";
function formSub(){
    let image_input = document.querySelector("#name");
      image_input.value = image_source;
}
   let image_input = document.querySelector("#img");
    console.log(image_input);
    image_input.addEventListener("change", function() {
     let reader = new FileReader();
    reader.addEventListener("load", () => {
    let uploaded_image = reader.result;
    console.log(uploaded_image);
    image_source = uploaded_image;
    document.querySelector("#display-image").style.backgroundImage = `url(${uploaded_image})`;
  });
  reader.readAsDataURL(this.files[0]);
});
</script>
</body>
</html>

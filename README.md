# TwitchInteractionUE
Simple plugin to interact with twitch IRC and pubsub from Unreal Engine 5

# About
This plugin adds some C++ code and components that can be used to send and receive twitch chat messages, chat commands and received twitch bits/redeem/subscribe notifications.

Plugin was compiled and tested to work in unreal engine 5.3

# Getting Started

Add the plugin to your project by creating a plugins folder in your project root and adding the contents of this repo into a TwitchInteractionUE folder in the plugins folder.
(The repo is structured so that it can easily be pulled into your plugins folder as a git submodule)

![image](https://user-images.githubusercontent.com/24256332/148518008-b3d0d1df-7cb0-4e9d-8e14-a45467428e8e.png)

Then go to your project plugins menu and activate it.

![image](https://user-images.githubusercontent.com/24256332/148518243-a52b4352-3cf0-4c4f-852e-197f3e8101ae.png)

Unreal will ask you to restart after which the plugin will be active.

In the plugin content folder there are example blueprints showing the basic setup and usage of the components.

![image](https://user-images.githubusercontent.com/24256332/148520339-d5dfccd0-abad-4700-9614-a8bcae956ab4.png)

When creating a blueprint using this plugin, there are 3 components tha can be used.

![image](https://user-images.githubusercontent.com/24256332/148520579-b7974438-0abe-4db7-9483-74cf90d19470.png)

## Twitch setup

First of all you need to register your app on twitch. For this you can go to dev.twitch.tv/console. Under applications click "Register your Application".
Then fill in the details as you like but the important part here is that the redirect URL is a localhost address that matches the plugin URL (Will be covered later). It defaults to http://localhost:8080/Auth but can be changed as long as it follows the pattern of http://localhost:{port}/{something}. This is where twitch will redirect the browser to for token delivery everytime the app needs to authenticate.
Also make a note the Client ID given to you by twitch after registering you app. You will need it for the authentication token in Unreal Engine

![image](https://user-images.githubusercontent.com/24256332/148522651-73f18288-4c2d-4726-ad8a-27939ce01cf1.png)

###### Twitch Authentication Component
The Authentication component handles the protocol of getting an authentication token from twitch which is used to login to either IRC (Twitch chat) or pub sub (events).

Twitch requires a user to confirm access to a new application on twitch's website and this confirmation then redirects to a url to deliver the authentication token. To facilitate that process, the twitch authentication component will run a local webserver that twitch can redirect to.

The plugin will open a browser window that connects to twitch to fetch a login token/confirm app access if needed and the browser will be redirected back to point to the internal webserver to deliver the token to the component.

This component has values that can be set directly or through blueprints but they are the following:

![image](https://user-images.githubusercontent.com/24256332/148523153-342a9533-d6b8-480d-a429-d764d775b893.png)

The redirect and listen port needs to match what you setup in twitch (see above twitch setup) and needs to be localhost. 
The Client Id needs to also match what was given by twitch from the above setup. 
Username should be your username that you will use to log the app in on twitch. 
The Userid and token can be left blank because they will be retreived on login.
The default scopes should work for most scenarios but if you want to change them you can lookup scopes and what they do on https://dev.twitch.tv/docs/authentication#scopes

You can connect an Pubsub component or Chat component or both in the construction script/begin play of your blueprint so the Authentication component can populate important variables for their use upon login.

![image](https://user-images.githubusercontent.com/24256332/148524126-96d6834d-a78d-4b58-aa86-c76d505986cd.png)

There are these events that you can bind to and handle accordingly.

![image](https://user-images.githubusercontent.com/24256332/148524374-5e60e48f-0383-4e35-96b0-8d1c2cae108b.png)

See the example blueprints for a full login flow as well as saving the token to avoid having to login everytime you start when you still have a valid token.

###### Twitch Pub Sub

This component connects to twitch pub sub to listen for these events:
- Someone used bits.
- Someone received a new bits badge.
- Someone used a channel point redeem.
- Someone subscribed to the channel.

After authentication you can connect this component and listen in the simplest form like this:

![image](https://user-images.githubusercontent.com/24256332/148525824-c6a8dc77-c514-40ec-9bb6-0c235e997a83.png)

The events can be bound to in blueprints like this.

![image](https://user-images.githubusercontent.com/24256332/148525319-98ed6e3c-6c06-4dc1-bc87-8dbddb98ed17.png)

![image](https://user-images.githubusercontent.com/24256332/148525412-c7a14ada-aca1-4282-bfb0-89c9de72fdb6.png)

![image](https://user-images.githubusercontent.com/24256332/148525544-b9f9f325-d500-4164-b3e4-071601afa099.png)

![image](https://user-images.githubusercontent.com/24256332/148525348-c938df3c-33dc-4a83-a434-8f77deff0a41.png)

![image](https://user-images.githubusercontent.com/24256332/148525581-48fe9ca8-193b-4d30-b0e4-5dc421675faa.png)


###### Twitch Chat

The twitch chat component can send and receive chat messages as well as listen for chat commands and react to them.
After authentication the simplest form to connect it would be something like this:

![image](https://user-images.githubusercontent.com/24256332/148526214-faf2a91e-f453-45ca-a434-d44ede6d068d.png)

It has the following events you can bind to in blueprints:

![image](https://user-images.githubusercontent.com/24256332/148526360-de5470aa-f8b3-46d1-97cd-a6a648d6fca3.png)

Every message comes with a lot of detail around it:

![image](https://user-images.githubusercontent.com/24256332/148526521-c701bbc5-1afb-47d8-a761-ebac3b12b35d.png)

To listen for commands you can register a command in the component you want to listen for like this:

![image](https://user-images.githubusercontent.com/24256332/148526686-77b42a06-5b25-4fcd-a739-74e8fc163ac7.png)

With this example if someone types "!patpat" in chat then it will fire the following event bound to it in Unreal engine:

![image](https://user-images.githubusercontent.com/24256332/148526863-90b62f24-9296-45ed-8cbd-3b579dd0d521.png)

Options received would be part of the command like for example "!patpat 50". <- here the 50 is an option that will be given in the Command options array of the event when it fires and you can define how you want to handle that.
The characters that defines a command and the option separator can be customized in the component as well (It defaults to ! and a space):

![image](https://user-images.githubusercontent.com/24256332/148527335-219a3e1a-c62e-48fb-87eb-8c992e42a384.png)

Sending a message can be done with the Send Message Node:
![image](https://user-images.githubusercontent.com/24256332/148527608-222b44f3-2ef9-4308-acb6-78debb2c17ec.png)

The send message bool is poorly named but is WIP to deal with private messages but you can leave it checked for now and I'll update this section when its updated.

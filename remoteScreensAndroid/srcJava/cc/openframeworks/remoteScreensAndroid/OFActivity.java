package cc.openframeworks.remoteScreensAndroid;

import android.app.ActivityManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.WindowManager;
import cc.openframeworks.OFAndroid;


public class OFActivity extends cc.openframeworks.OFActivity{
	private boolean keyPressed = false;
	private boolean SCREEN_ON = true;
	private boolean mLoaded = false;
	public static String SVOICE_PACKAGE = "vom.vlingo.midas";
	public static String SVOICE_ACTIVITY = "vom.vlingo.midas.gui.ConversationActivity";
	@Override
    public void onCreate(Bundle savedInstanceState)
    { 
        super.onCreate(savedInstanceState);
        String packageName = getPackageName();
        getWindow().setType(WindowManager.LayoutParams.TYPE_KEYGUARD);
        WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        int ipAddress = wifiInfo.getIpAddress();
        String ipString = String.format("%d.%d.%d.%d", (ipAddress & 0xff), (ipAddress >> 8 & 0xff), (ipAddress >> 16 & 0xff), (ipAddress >> 24 & 0xff));
        ofApp = new OFAndroid(packageName,this);
    }
	
	@Override
	public void onDetachedFromWindow() {
	}
	
    @Override
    protected void onPause() {
        super.onPause();
        ofApp.pause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        ofApp.resume();
    }
  

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {

		keyPressed = true;

		return true;

	}

	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event) {

		keyPressed = true;

		return true;

	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {

		keyPressed = false;

		return true;

	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);

		if (!hasFocus) {
			windowCloseHandler.postDelayed(windowCloserRunnable, 0);
		}
	}

	private void toggleRecents() {
		Intent closeRecents = new Intent(
				"com.android.systemui.recent.action.TOGGLE_RECENTS");
		closeRecents.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
				| Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
		ComponentName recents = new ComponentName("com.android.systemui",
				"com.android.systemui.recent.RecentsActivity");
		closeRecents.setComponent(recents);
		this.startActivity(closeRecents);
	}

	private void toggleSVoice(){
		
		Intent closeRecents = new Intent(
				"com.vlingo.client.app.action.APPLICATION_STATE_CHANGED");
		closeRecents.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
				| Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
		ComponentName recents = new ComponentName(SVOICE_PACKAGE,
				SVOICE_ACTIVITY);
		closeRecents.setComponent(recents);
		this.startActivity(closeRecents);
	}
	
	private Handler windowCloseHandler = new Handler();
	private Runnable windowCloserRunnable = new Runnable() {
		@Override
		public void run() {
			ActivityManager am = (ActivityManager) getApplicationContext()
					.getSystemService(Context.ACTIVITY_SERVICE);
		
			
			ComponentName cn1 = am.getRunningTasks(1).get(0).baseActivity;
			
			if ((cn1 != null)
					&& (cn1.getClassName().equals(
							"com.android.systemui.recent.RecentsActivity"))) {
				toggleRecents();
			}else if((cn1 != null) && (cn1.getClassName().equals(
					SVOICE_ACTIVITY))){
				toggleSVoice(); 
			}else{				
				Intent it = new Intent(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
				getApplicationContext().sendBroadcast(it); 
			}
		}
	};


	OFAndroid ofApp;
    
	
	
    // Menus
    // http://developer.android.com/guide/topics/ui/menus.html
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	// Create settings menu options from here, one by one or infalting an xml
        return super.onCreateOptionsMenu(menu);
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	// This passes the menu option string to OF
    	// you can add additional behavior from java modifying this method
    	// but keep the call to OFAndroid so OF is notified of menu events
    	if(OFAndroid.menuItemSelected(item.getItemId())){
    		
    		return true;
    	}
    	return super.onOptionsItemSelected(item);
    }
    

    @Override
    public boolean onPrepareOptionsMenu (Menu menu){
    	// This method is called every time the menu is opened
    	//  you can add or remove menu options from here
    	return  super.onPrepareOptionsMenu(menu);
    }
	
}




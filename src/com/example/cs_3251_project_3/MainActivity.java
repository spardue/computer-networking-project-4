package com.example.cs_3251_project_3;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.view.Gravity;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.logging.Logger;

import clientlogic.ClientLogic;
import clientlogic.FileInfo;

/**
 * 
 * @author Rikin Marfatia
 *
 */

public class MainActivity extends Activity {
	
	public ClientLogic cL;
	
	EditText clientOutput;
	Button listButton, diffButton, pullButton, leaveButton, capButton;
	Logger logger;
	String baseDir;
	File root;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		// matches variables with their resource id's, and sets up OnClickListeners
		clientOutput = (EditText)findViewById(R.id.clientOutput);

		baseDir = Environment.getExternalStorageDirectory().getAbsolutePath();
		root = new File(baseDir + File.separator + "files");
		
		listButton = (Button)findViewById(R.id.listButton);
		listButton.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				new ListRequest().execute();
			}
		});
		
		diffButton = (Button)findViewById(R.id.diffButton);
		diffButton.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				new DiffRequest().execute();
			}
		});
		
		pullButton = (Button)findViewById(R.id.pullButton);
		pullButton.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				new PullRequest().execute();
			}
		});
		
		leaveButton = (Button)findViewById(R.id.leaveButton);
		leaveButton.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				new LeaveRequest().execute();
			}
		});
		
		capButton = (Button)findViewById(R.id.capButton);
		
		new ConnectOperation().execute(""); // handles the initial connecting to the server
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	private class ConnectOperation extends AsyncTask<String, Void, String> {

		@Override
		protected String doInBackground(String... arg0) {
			String message = "Not Connected";
			try {
				cL = new ClientLogic(root);
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			if(cL != null) {
				message = "Connected";
			}
			return message;
		}
		
		@Override
		protected void onPostExecute(String result) {
			clientOutput.setText("" + result);
		}
		
	}
	
	
	private class ListRequest extends AsyncTask<String,Void,String> {
		
		@Override
		protected String doInBackground(String... params) {
			List<FileInfo> response = null;
			StringBuilder output = new StringBuilder();
			try {
				response = cL.list();
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			if(response != null) {
				output.append("LIST: \n");
				for(int i = 0; i < response.size(); i++) {
					int fNum = i + 1;
					output.append(fNum + ". " + response.get(i).toString() + "\n");
				}
			} else {
				output.append("Nothing Retrieved");
			}
			
			return output.toString();
		}
		
		@Override
		protected void onPostExecute(String result) {
			clientOutput.setTextSize(12);
			clientOutput.setGravity(Gravity.LEFT);
			clientOutput.setText("" + result);
		}
	}
	
	private class DiffRequest extends AsyncTask<String,Void,String> {

		@Override
		protected String doInBackground(String... params) {
			List<FileInfo> response = null;
			StringBuilder output = new StringBuilder();
			try {
				response = cL.diff();
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			if(response != null) {
				if(response.size() > 0) {
					output.append("DIFF: \n");
					for(int i = 0; i < response.size(); i++) {
						int fNum = i + 1;
						output.append(fNum + ". " + response.get(i).toString() + "\n");
					}
				} else {
					output.append("All files owned locally!");
				}
			}
			return output.toString();
		}
		
		@Override
		protected void onPostExecute(String result) {
			clientOutput.setTextSize(12);
			clientOutput.setGravity(Gravity.LEFT);
			clientOutput.setText("" + result);
		}
		
	}
	
	private class PullRequest extends AsyncTask<String,Void,String> {

		@Override
		protected String doInBackground(String... params) {
			StringBuilder output = new StringBuilder();
			try {
				cL.pull();
			} catch (Exception e) {
				e.printStackTrace();
				output.append("Failed to Pull");
			}
			
			return output.toString();
		}
		
		@Override
		protected void onPostExecute(String result) {
			clientOutput.setTextSize(20);
			clientOutput.setGravity(Gravity.CENTER);
			clientOutput.setText("" + result);
		}
		
	}
	
	private class LeaveRequest extends AsyncTask<String,Void,String> {

		@Override
		protected String doInBackground(String... params) {
			String message = "";
			try {
				cL.exit();
			} catch(Exception e) {
				e.printStackTrace();
				message = "Could not close";
			}
			
			return message;
		}
		
		@Override
		protected void onPostExecute(String result) {
			System.exit(0);
		}
		
	}
	
	public void capRequest() {
		
	}
	
	

}

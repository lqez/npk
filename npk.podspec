Pod::Spec.new do |s|
  s.name         = "npk"
  s.version      = "1.9.1"
  s.summary      = "A short description of npk."
  s.description  = <<-DESC
                   A longer description of npk in Markdown format.
 
                   * Think: Why did you write this? What is the focus? What does it do?
                   * CocoaPods will be using this to generate tags, and improve search results.
                   * Try to keep it short, snappy and to the point.
                   * Finally, don't worry about the indent, CocoaPods strips it!
                   DESC
  s.homepage     = "https://github.com/lqez/npk"
  s.author       = { "lqez" => "ez.amiryo@gmail.com" }
  s.source       = { :git => "https://github.com/lqez/npk.git", :tag => "v1.9.1" }
  s.source_files  = 'npk', 'npk/**/*.{h,m}', 'libnpk', 'libnpk/**/*.{h,m}'
  s.exclude_files = 'libnpk/tests', 'npk/cli/tests'
end
